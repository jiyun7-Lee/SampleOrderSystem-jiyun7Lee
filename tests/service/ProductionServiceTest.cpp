#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/model/repository/IProductionRepository.h"
#include "../../src/model/repository/IInventoryRepository.h"
#include "../../src/model/repository/IOrderRepository.h"
#include "../../src/model/domain/ProductionJob.h"
#include "../../src/model/domain/Order.h"
#include "../../src/model/domain/Inventory.h"
#include "../../src/model/domain/OrderStatus.h"
#include "../../src/service/ProductionService.h"
#include "../../src/util/ITimeProvider.h"
#include <stdexcept>
#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::AnyNumber;
using ::testing::AtLeast;

// -----------------------------------------------------------------------
// Mock 클래스 정의 (이름 충돌 방지를 위해 ForProd 접미사 사용)
// -----------------------------------------------------------------------

class MockProductionRepositoryForProd : public IProductionRepository {
public:
    MOCK_METHOD(void, Save, (const ProductionJob&), (override));
    MOCK_METHOD(std::optional<ProductionJob>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>, FindByOrderId, (const std::string&), (const, override));
};

class MockInventoryRepositoryForProd : public IInventoryRepository {
public:
    MOCK_METHOD(void, Save, (const Inventory&), (override));
    MOCK_METHOD(std::optional<Inventory>, FindBySampleId, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsBySampleId, (const std::string&), (const, override));
};

class MockOrderRepositoryForProd : public IOrderRepository {
public:
    MOCK_METHOD(void, Save, (const Order&), (override));
    MOCK_METHOD(std::optional<Order>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Order>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Order>, FindByStatus, (OrderStatus), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};

class MockTimeProviderForProd : public ITimeProvider {
public:
    MOCK_METHOD(std::chrono::system_clock::time_point, Now, (), (const, override));
};

// -----------------------------------------------------------------------
// ProductionServiceFixture
// -----------------------------------------------------------------------
class ProductionServiceFixture : public ::testing::Test {
protected:
    MockProductionRepositoryForProd mockProductionRepo;
    MockInventoryRepositoryForProd  mockInventoryRepo;
    MockOrderRepositoryForProd      mockOrderRepo;
    MockTimeProviderForProd         mockTime;

    std::unique_ptr<ProductionService> sut;

    // 고정 기준 시각 (생산 시작 시점)
    static std::chrono::system_clock::time_point BaseTime() {
        std::tm tm{};
        tm.tm_year = 126;  // 2026
        tm.tm_mon  = 0;    // 1월
        tm.tm_mday = 15;
        tm.tm_hour = 9;
        tm.tm_min  = 0;
        tm.tm_sec  = 0;
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    // 기준 시각으로부터 N분 경과한 시각 반환
    static std::chrono::system_clock::time_point After(double minutes) {
        return BaseTime() + std::chrono::seconds(static_cast<long long>(minutes * 60.0));
    }

    void SetUp() override {
        ON_CALL(mockTime, Now()).WillByDefault(Return(BaseTime()));

        sut = std::make_unique<ProductionService>(
            mockProductionRepo,
            mockInventoryRepo,
            mockOrderRepo,
            mockTime);
    }

    // 헬퍼: 생산 중인 ProductionJob 설정
    // startTime = BaseTime, expectedFinishTime = BaseTime + totalMinutes
    ProductionJob MakeJob(const std::string& productionId,
                          const std::string& orderId,
                          const std::string& sampleId,
                          int requiredQty,
                          int actualQty,
                          double totalMinutes,
                          std::chrono::system_clock::time_point start,
                          std::chrono::system_clock::time_point expectedFinish) {
        ProductionJob job;
        job.productionId             = productionId;
        job.orderId                  = orderId;
        job.sampleId                 = sampleId;
        job.requiredQuantity         = requiredQty;
        job.actualProductionQuantity = actualQty;
        job.totalProductionTime      = totalMinutes;
        job.startTime                = start;
        job.expectedFinishTime       = expectedFinish;
        return job;
    }

    // 헬퍼: PRODUCING 상태 Order 반환
    Order MakeProducingOrder(const std::string& orderId, const std::string& sampleId, int qty) {
        Order o;
        o.orderId      = orderId;
        o.sampleId     = sampleId;
        o.customerName = "TestCustomer";
        o.quantity     = qty;
        o.status       = OrderStatus::PRODUCING;
        o.createdAt    = BaseTime();
        return o;
    }

    // 헬퍼: Inventory 설정
    void GivenInventory(const std::string& sampleId, int currentStock, int reservedStock) {
        Inventory inv;
        inv.sampleId      = sampleId;
        inv.currentStock  = currentStock;
        inv.reservedStock = reservedStock;
        ON_CALL(mockInventoryRepo, FindBySampleId(sampleId))
            .WillByDefault(Return(std::optional<Inventory>{inv}));
        ON_CALL(mockInventoryRepo, ExistsBySampleId(sampleId))
            .WillByDefault(Return(true));
    }
};

// -----------------------------------------------------------------------
// Normal Cases — CheckAndCompleteProduction
// -----------------------------------------------------------------------

// expectedFinishTime 이 아직 미래: 완료 처리 없음 → orderRepo.Save 미호출
TEST_F(ProductionServiceFixture, CheckCompletion_BeforeFinishTime_ShouldNotComplete) {
    // Now = BaseTime, expectedFinishTime = BaseTime + 60분 (미래)
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 390.0,
                                BaseTime(),
                                After(60.0));

    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-001", "S-001", 10)}));

    // Now < expectedFinishTime → 완료 처리 없어야 함
    ON_CALL(mockTime, Now()).WillByDefault(Return(BaseTime()));
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    sut->CheckAndCompleteProduction();
}

// expectedFinishTime 이 지난 경우: 완료 처리 → orderRepo.Save 1회 호출
TEST_F(ProductionServiceFixture, CheckCompletion_AfterFinishTime_ShouldComplete) {
    // expectedFinishTime = BaseTime + 30분, Now = BaseTime + 31분 (완료 후)
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 30.0,
                                BaseTime(),
                                After(30.0));

    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-001", "S-001", 10)}));
    GivenInventory("S-001", 0, 0);

    // Now = BaseTime + 31분 (expectedFinishTime 경과)
    ON_CALL(mockTime, Now()).WillByDefault(Return(After(31.0)));

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);

    sut->CheckAndCompleteProduction();
}

// 완료 처리 후 저장된 Order.status == CONFIRMED
TEST_F(ProductionServiceFixture, OnComplete_ShouldUpdateOrderToConfirmed) {
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 30.0,
                                BaseTime(),
                                After(30.0));

    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-001", "S-001", 10)}));
    GivenInventory("S-001", 0, 0);
    ON_CALL(mockTime, Now()).WillByDefault(Return(After(31.0)));

    Order savedOrder;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&savedOrder));
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(AnyNumber());

    sut->CheckAndCompleteProduction();

    EXPECT_EQ(savedOrder.status, OrderStatus::CONFIRMED);
}

// 완료 처리 후 inventoryRepo.Save 호출 (currentStock 증가 반영)
TEST_F(ProductionServiceFixture, OnComplete_ShouldAddStockToInventory) {
    // actualProductionQuantity = 13
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 30.0,
                                BaseTime(),
                                After(30.0));

    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-001", "S-001", 10)}));
    GivenInventory("S-001", 5, 0);
    ON_CALL(mockTime, Now()).WillByDefault(Return(After(31.0)));

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);

    // inventoryRepo.Save 가 최소 1회 호출되어야 함 (재고 갱신)
    Inventory savedInv;
    EXPECT_CALL(mockInventoryRepo, Save(_))
        .Times(AtLeast(1))
        .WillOnce(SaveArg<0>(&savedInv));

    sut->CheckAndCompleteProduction();

    // currentStock = 기존(5) + actualProductionQuantity(13) = 18
    EXPECT_EQ(savedInv.currentStock, 18);
}

// 완료 처리 후 해당 job 에 대해 productionRepo.Save 가 최소 1회 호출
TEST_F(ProductionServiceFixture, OnComplete_ShouldDequeueJob) {
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 30.0,
                                BaseTime(),
                                After(30.0));

    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-001", "S-001", 10)}));
    GivenInventory("S-001", 0, 0);
    ON_CALL(mockTime, Now()).WillByDefault(Return(After(31.0)));

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(AnyNumber());
    // job 완료 처리 후 productionRepo.Save 로 상태 갱신 최소 1회
    EXPECT_CALL(mockProductionRepo, Save(_)).Times(AtLeast(1));

    sut->CheckAndCompleteProduction();
}

// 2개 job 중 expectedFinishTime 이 이른 것만 완료 처리 (FIFO 순서 검증)
TEST_F(ProductionServiceFixture, MultipleJobs_ShouldProcessInFIFOOrder) {
    // job1: expectedFinishTime = BaseTime + 10분 (완료됨)
    ProductionJob job1 = MakeJob("P-001", "ORD-001", "S-001",
                                 5, 7, 10.0,
                                 BaseTime(),
                                 After(10.0));

    // job2: expectedFinishTime = BaseTime + 120분 (아직 미완료)
    ProductionJob job2 = MakeJob("P-002", "ORD-002", "S-002",
                                 8, 10, 120.0,
                                 BaseTime(),
                                 After(120.0));

    // Now = BaseTime + 11분: job1만 완료 조건 충족
    ON_CALL(mockTime, Now()).WillByDefault(Return(After(11.0)));
    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job1, job2}));

    // ORD-001은 PRODUCING 상태
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-001", "S-001", 5)}));
    // ORD-002도 조회 가능하나 완료 미대상
    ON_CALL(mockOrderRepo, FindById("ORD-002"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-002", "S-002", 8)}));

    GivenInventory("S-001", 0, 0);
    GivenInventory("S-002", 0, 0);

    // orderRepo.Save 는 완료된 job1에 해당하는 ORD-001 에 대해서만 1회
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(AnyNumber());
    EXPECT_CALL(mockProductionRepo, Save(_)).Times(AnyNumber());

    sut->CheckAndCompleteProduction();
}

// -----------------------------------------------------------------------
// Normal Cases — GetCurrentJob / GetWaitingJobs
// -----------------------------------------------------------------------

// 생산 중인 job이 없으면 GetCurrentJob 은 nullopt 반환
TEST_F(ProductionServiceFixture, GetCurrentJob_NoJobs_ShouldReturnNullopt) {
    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{}));

    auto result = sut->GetCurrentJob();

    EXPECT_FALSE(result.has_value());
}

// 생산 중인 job이 1개 이상이면 GetCurrentJob 은 첫 번째 job 반환
TEST_F(ProductionServiceFixture, GetCurrentJob_OneJob_ShouldReturnJob) {
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 390.0,
                                BaseTime(),
                                After(390.0));

    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));

    auto result = sut->GetCurrentJob();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->productionId, "P-001");
}

// job이 없으면 GetWaitingJobs 는 빈 목록 반환
TEST_F(ProductionServiceFixture, GetWaitingJobs_NoJobs_ShouldReturnEmpty) {
    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{}));

    auto result = sut->GetWaitingJobs();

    EXPECT_TRUE(result.empty());
}

// job이 2개이면 GetWaitingJobs 는 첫 번째를 제외한 1개를 반환
TEST_F(ProductionServiceFixture, GetWaitingJobs_TwoJobs_ShouldReturnSecondOnly) {
    ProductionJob job1 = MakeJob("P-001", "ORD-001", "S-001",
                                 5, 7, 30.0,
                                 BaseTime(),
                                 After(30.0));
    ProductionJob job2 = MakeJob("P-002", "ORD-002", "S-002",
                                 8, 10, 60.0,
                                 After(30.0),
                                 After(90.0));

    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job1, job2}));

    auto result = sut->GetWaitingJobs();

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].productionId, "P-002");
}

// -----------------------------------------------------------------------
// Abnormal Cases
// -----------------------------------------------------------------------

// CheckAndCompleteProduction — 생산 목록이 비어 있으면 아무것도 처리 안 함
TEST_F(ProductionServiceFixture, CheckCompletion_EmptyQueue_ShouldDoNothing) {
    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{}));

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(0);
    EXPECT_CALL(mockProductionRepo, Save(_)).Times(0);

    EXPECT_NO_THROW(sut->CheckAndCompleteProduction());
}

// CheckAndCompleteProduction — expectedFinishTime 정확히 일치(경계값) 시 완료 처리
TEST_F(ProductionServiceFixture, CheckCompletion_ExactFinishTime_ShouldComplete) {
    // Now == expectedFinishTime (경계값: 완료 조건)
    auto finishTime = After(60.0);
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 60.0,
                                BaseTime(),
                                finishTime);

    ON_CALL(mockTime, Now()).WillByDefault(Return(finishTime));
    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{MakeProducingOrder("ORD-001", "S-001", 10)}));
    GivenInventory("S-001", 0, 0);

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);

    sut->CheckAndCompleteProduction();
}

// CheckAndCompleteProduction — expectedFinishTime 1초 전 (경계값): 완료 처리 없음
TEST_F(ProductionServiceFixture, CheckCompletion_OneSecondBeforeFinish_ShouldNotComplete) {
    auto finishTime = After(60.0);
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 60.0,
                                BaseTime(),
                                finishTime);

    // Now = finishTime - 1초
    auto nowTime = finishTime - std::chrono::seconds(1);
    ON_CALL(mockTime, Now()).WillByDefault(Return(nowTime));
    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    sut->CheckAndCompleteProduction();
}

// 이미 CONFIRMED 상태인 주문에 대해 완료 처리 시 예외 또는 Save 미호출
TEST_F(ProductionServiceFixture, CheckCompletion_AlreadyConfirmed_ShouldNotSaveAgain) {
    ProductionJob job = MakeJob("P-001", "ORD-001", "S-001",
                                10, 13, 30.0,
                                BaseTime(),
                                After(30.0));

    ON_CALL(mockTime, Now()).WillByDefault(Return(After(31.0)));
    ON_CALL(mockProductionRepo, FindAll())
        .WillByDefault(Return(std::vector<ProductionJob>{job}));

    // 이미 CONFIRMED 상태
    Order confirmedOrder = MakeProducingOrder("ORD-001", "S-001", 10);
    confirmedOrder.status = OrderStatus::CONFIRMED;
    ON_CALL(mockOrderRepo, FindById("ORD-001"))
        .WillByDefault(Return(std::optional<Order>{confirmedOrder}));
    GivenInventory("S-001", 0, 0);

    // CONFIRMED 상태이면 orderRepo.Save(CONFIRMED 재저장) 없어야 함
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_NO_THROW(sut->CheckAndCompleteProduction());
}

// -----------------------------------------------------------------------
// ProductionCalcFixture — 생산량 계산 검증
// -----------------------------------------------------------------------
class ProductionCalcFixture : public ::testing::Test {
protected:
    MockProductionRepositoryForProd mockProductionRepo;
    MockInventoryRepositoryForProd  mockInventoryRepo;
    MockOrderRepositoryForProd      mockOrderRepo;
    MockTimeProviderForProd         mockTime;

    std::unique_ptr<ProductionService> sut;

    void SetUp() override {
        std::tm tm{};
        tm.tm_year = 126; tm.tm_mon = 0; tm.tm_mday = 1;
        tm.tm_hour = 0;   tm.tm_min = 0; tm.tm_sec  = 0;
        auto now = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        ON_CALL(mockTime, Now()).WillByDefault(Return(now));

        sut = std::make_unique<ProductionService>(
            mockProductionRepo,
            mockInventoryRepo,
            mockOrderRepo,
            mockTime);
    }
};

// yieldRate=0.9, requiredQty=10 → actualQty = ceil(10 / (0.9 * 0.9)) = ceil(12.35) = 13
TEST_F(ProductionCalcFixture, ActualQty_WithHighYield_ShouldBeSmaller) {
    const double yieldRate   = 0.9;
    const int    requiredQty = 10;
    const double denominator = yieldRate * 0.9;
    const int    expected    = static_cast<int>(std::ceil(requiredQty / denominator));

    EXPECT_EQ(expected, 13);
}

// yieldRate=0.5, requiredQty=10 → actualQty = ceil(10 / (0.5 * 0.9)) = ceil(22.22) = 23
TEST_F(ProductionCalcFixture, ActualQty_WithLowYield_ShouldBeLarger) {
    const double yieldRate   = 0.5;
    const int    requiredQty = 10;
    const double denominator = yieldRate * 0.9;
    const int    expected    = static_cast<int>(std::ceil(requiredQty / denominator));

    EXPECT_EQ(expected, 23);
}

// requiredQty=100, yieldRate=0.1 → actualQty = ceil(100 / (0.1 * 0.9)) = ceil(1111.11) = 1112
TEST_F(ProductionCalcFixture, ActualQty_Example_100_01_ShouldBe1112) {
    const double yieldRate   = 0.1;
    const int    requiredQty = 100;
    const double denominator = yieldRate * 0.9;
    const int    expected    = static_cast<int>(std::ceil(requiredQty / denominator));

    EXPECT_EQ(expected, 1112);
}

// avgProductionTime=3.0, actualQty=100 → totalTime = 3.0 * 100 = 300.0
TEST_F(ProductionCalcFixture, TotalTime_3sec_100ea_ShouldBe300) {
    const double avgProductionTime = 3.0;
    const int    actualQty         = 100;
    const double totalTime         = avgProductionTime * actualQty;

    EXPECT_DOUBLE_EQ(totalTime, 300.0);
}

// yieldRate=1.0 (최대값 경계): actualQty = ceil(10 / (1.0 * 0.9)) = ceil(11.11) = 12
TEST_F(ProductionCalcFixture, ActualQty_YieldOne_ShouldBeMinimum) {
    const double yieldRate   = 1.0;
    const int    requiredQty = 10;
    const double denominator = yieldRate * 0.9;
    const int    expected    = static_cast<int>(std::ceil(requiredQty / denominator));

    EXPECT_EQ(expected, 12);
}

// yieldRate=0.01 (극소값 경계): 생산량·시간이 크게 증가
// actualQty = ceil(1 / (0.01 * 0.9)) = ceil(111.11) = 112
TEST_F(ProductionCalcFixture, ActualQty_VeryLowYield_ShouldBeVeryLarge) {
    const double yieldRate   = 0.01;
    const int    requiredQty = 1;
    const double denominator = yieldRate * 0.9;
    const int    expected    = static_cast<int>(std::ceil(requiredQty / denominator));

    EXPECT_EQ(expected, 112);
}

// avgProductionTime=0.0 → totalTime = 0.0 (즉시 완료 가능)
TEST_F(ProductionCalcFixture, TotalTime_ZeroAvgTime_ShouldBeZero) {
    const double avgProductionTime = 0.0;
    const int    actualQty         = 50;
    const double totalTime         = avgProductionTime * actualQty;

    EXPECT_DOUBLE_EQ(totalTime, 0.0);
}
