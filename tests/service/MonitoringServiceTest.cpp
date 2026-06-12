#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/model/repository/IOrderRepository.h"
#include "../../src/model/repository/IInventoryRepository.h"
#include "../../src/model/repository/ISampleRepository.h"
#include "../../src/model/domain/Order.h"
#include "../../src/model/domain/Inventory.h"
#include "../../src/model/domain/Sample.h"
#include "../../src/model/domain/OrderStatus.h"
#include "../../src/service/MonitoringService.h"
#include <memory>
#include <optional>
#include <vector>
#include <string>

using ::testing::Return;
using ::testing::_;
using ::testing::AnyNumber;

// -----------------------------------------------------------------------
// Mock 클래스 — 기존 파일과 이름 충돌 방지를 위해 ForMonitoring 접미사 사용
// -----------------------------------------------------------------------
class MockOrderRepositoryForMonitoring : public IOrderRepository {
public:
    MOCK_METHOD(void, Save, (const Order&), (override));
    MOCK_METHOD(std::optional<Order>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Order>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Order>, FindByStatus, (OrderStatus), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};

class MockInventoryRepositoryForMonitoring : public IInventoryRepository {
public:
    MOCK_METHOD(void, Save, (const Inventory&), (override));
    MOCK_METHOD(std::optional<Inventory>, FindBySampleId, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsBySampleId, (const std::string&), (const, override));
};

class MockSampleRepositoryForMonitoring : public ISampleRepository {
public:
    MOCK_METHOD(void, Save, (const Sample&), (override));
    MOCK_METHOD(std::optional<Sample>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindByName, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};

// -----------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------
class MonitoringServiceFixture : public ::testing::Test {
protected:
    MockOrderRepositoryForMonitoring      mockOrderRepo;
    MockInventoryRepositoryForMonitoring  mockInventoryRepo;
    MockSampleRepositoryForMonitoring     mockSampleRepo;

    std::unique_ptr<MonitoringService> sut;

    void SetUp() override {
        sut = std::make_unique<MonitoringService>(
            mockOrderRepo, mockInventoryRepo, mockSampleRepo);
    }

    // 헬퍼: 특정 상태에 해당하는 Order 목록을 반환하도록 설정
    void GivenOrdersByStatus(OrderStatus status, std::vector<Order> orders) {
        ON_CALL(mockOrderRepo, FindByStatus(status))
            .WillByDefault(Return(orders));
    }

    // 헬퍼: Order 빌더
    Order MakeOrder(const std::string& orderId,
                    const std::string& sampleId,
                    int quantity,
                    OrderStatus status) {
        Order o;
        o.orderId      = orderId;
        o.sampleId     = sampleId;
        o.customerName = "TestCustomer";
        o.quantity     = quantity;
        o.status       = status;
        return o;
    }

    // 헬퍼: Sample 빌더
    Sample MakeSample(const std::string& sampleId, const std::string& name) {
        Sample s;
        s.sampleId              = sampleId;
        s.name                  = name;
        s.averageProductionTime = 30.0;
        s.yieldRate             = 0.9;
        return s;
    }

    // 헬퍼: Inventory 빌더
    Inventory MakeInventory(const std::string& sampleId,
                            int currentStock,
                            int reservedStock) {
        Inventory inv;
        inv.sampleId      = sampleId;
        inv.currentStock  = currentStock;
        inv.reservedStock = reservedStock;
        return inv;
    }

    // 헬퍼: 시료 목록과 재고를 함께 설정
    void GivenSampleWithInventory(const std::string& sampleId,
                                  const std::string& sampleName,
                                  int currentStock,
                                  int reservedStock) {
        ON_CALL(mockSampleRepo, FindAll())
            .WillByDefault(Return(std::vector<Sample>{MakeSample(sampleId, sampleName)}));
        ON_CALL(mockInventoryRepo, FindBySampleId(sampleId))
            .WillByDefault(Return(std::optional<Inventory>{
                MakeInventory(sampleId, currentStock, reservedStock)}));
    }
};

// -----------------------------------------------------------------------
// Normal Cases — GetOrderSummary
// -----------------------------------------------------------------------

// REJECTED 상태 주문은 집계에서 제외되어야 한다
TEST_F(MonitoringServiceFixture, GetOrderSummary_ShouldExcludeRejected) {
    // REJECTED 주문 2건이 있더라도 집계에 포함되지 않아야 함
    GivenOrdersByStatus(OrderStatus::RESERVED,  {MakeOrder("ORD-001", "S-001", 5, OrderStatus::RESERVED)});
    GivenOrdersByStatus(OrderStatus::PRODUCING, {});
    GivenOrdersByStatus(OrderStatus::CONFIRMED, {});
    GivenOrdersByStatus(OrderStatus::RELEASE,   {});
    // REJECTED 는 조회 자체를 요청하지 않아야 함
    EXPECT_CALL(mockOrderRepo, FindByStatus(OrderStatus::REJECTED)).Times(0);

    auto summary = sut->GetOrderSummary();

    EXPECT_EQ(summary.reservedCount,  1);
    EXPECT_EQ(summary.producingCount, 0);
    EXPECT_EQ(summary.confirmedCount, 0);
    EXPECT_EQ(summary.releaseCount,   0);
}

// 각 상태별 주문 수가 정확히 집계되어야 한다
TEST_F(MonitoringServiceFixture, GetOrderSummary_CountsCorrectByStatus) {
    GivenOrdersByStatus(OrderStatus::RESERVED, {
        MakeOrder("ORD-001", "S-001", 5, OrderStatus::RESERVED),
        MakeOrder("ORD-002", "S-002", 3, OrderStatus::RESERVED)
    });
    GivenOrdersByStatus(OrderStatus::PRODUCING, {
        MakeOrder("ORD-003", "S-001", 10, OrderStatus::PRODUCING)
    });
    GivenOrdersByStatus(OrderStatus::CONFIRMED, {
        MakeOrder("ORD-004", "S-002", 7, OrderStatus::CONFIRMED),
        MakeOrder("ORD-005", "S-001", 2, OrderStatus::CONFIRMED),
        MakeOrder("ORD-006", "S-002", 4, OrderStatus::CONFIRMED)
    });
    GivenOrdersByStatus(OrderStatus::RELEASE, {
        MakeOrder("ORD-007", "S-001", 1, OrderStatus::RELEASE)
    });

    auto summary = sut->GetOrderSummary();

    EXPECT_EQ(summary.reservedCount,  2);
    EXPECT_EQ(summary.producingCount, 1);
    EXPECT_EQ(summary.confirmedCount, 3);
    EXPECT_EQ(summary.releaseCount,   1);
}

// -----------------------------------------------------------------------
// Normal Cases — GetInventoryStatus
// -----------------------------------------------------------------------

// currentStock == 0 이면 DEPLETED 상태 반환
TEST_F(MonitoringServiceFixture, GetInventoryStatus_ZeroStock_ShouldBeDepleted) {
    // currentStock=0 → DEPLETED
    GivenSampleWithInventory("S-001", "AlphaSample", 0, 0);

    auto statuses = sut->GetInventoryStatus();

    ASSERT_EQ(statuses.size(), 1u);
    EXPECT_EQ(statuses[0].sampleId,     "S-001");
    EXPECT_EQ(statuses[0].sampleName,   "AlphaSample");
    EXPECT_EQ(statuses[0].currentStock, 0);
    EXPECT_EQ(statuses[0].status,       StockStatus::DEPLETED);
}

// availableStock <= 0 (currentStock > 0, reservedStock >= currentStock) 이면 SHORTAGE
TEST_F(MonitoringServiceFixture, GetInventoryStatus_InsufficientAvailable_ShouldBeShortage) {
    // currentStock=5, reservedStock=5 → availableStock=0 → SHORTAGE
    GivenSampleWithInventory("S-002", "BetaSample", 5, 5);

    auto statuses = sut->GetInventoryStatus();

    ASSERT_EQ(statuses.size(), 1u);
    EXPECT_EQ(statuses[0].sampleId,     "S-002");
    EXPECT_EQ(statuses[0].sampleName,   "BetaSample");
    EXPECT_EQ(statuses[0].currentStock, 5);
    EXPECT_EQ(statuses[0].status,       StockStatus::SHORTAGE);
}

// availableStock > 0 이면 SUFFICIENT 상태 반환
TEST_F(MonitoringServiceFixture, GetInventoryStatus_SufficientAvailable_ShouldBeSufficient) {
    // currentStock=100, reservedStock=10 → availableStock=90 → SUFFICIENT
    GivenSampleWithInventory("S-003", "GammaSample", 100, 10);

    auto statuses = sut->GetInventoryStatus();

    ASSERT_EQ(statuses.size(), 1u);
    EXPECT_EQ(statuses[0].sampleId,     "S-003");
    EXPECT_EQ(statuses[0].sampleName,   "GammaSample");
    EXPECT_EQ(statuses[0].currentStock, 100);
    EXPECT_EQ(statuses[0].status,       StockStatus::SUFFICIENT);
}

// -----------------------------------------------------------------------
// Boundary Cases — GetInventoryStatus
// -----------------------------------------------------------------------

// availableStock == 1 (경계: 딱 1개 남음) 이면 SUFFICIENT
TEST_F(MonitoringServiceFixture, GetInventoryStatus_OneUnitAvailable_ShouldBeSufficient) {
    // currentStock=10, reservedStock=9 → availableStock=1 → SUFFICIENT
    GivenSampleWithInventory("S-004", "DeltaSample", 10, 9);

    auto statuses = sut->GetInventoryStatus();

    ASSERT_EQ(statuses.size(), 1u);
    EXPECT_EQ(statuses[0].status, StockStatus::SUFFICIENT);
}

// reservedStock > currentStock (예약 초과) 이면 availableStock < 0 → SHORTAGE
TEST_F(MonitoringServiceFixture, GetInventoryStatus_ReservedExceedsStock_ShouldBeShortage) {
    // currentStock=3, reservedStock=5 → availableStock=-2 → SHORTAGE
    GivenSampleWithInventory("S-005", "EpsilonSample", 3, 5);

    auto statuses = sut->GetInventoryStatus();

    ASSERT_EQ(statuses.size(), 1u);
    EXPECT_EQ(statuses[0].status, StockStatus::SHORTAGE);
}

// -----------------------------------------------------------------------
// Abnormal Cases
// -----------------------------------------------------------------------

// 시료가 없는 경우 GetInventoryStatus 는 빈 목록 반환
TEST_F(MonitoringServiceFixture, GetInventoryStatus_NoSamples_ShouldReturnEmpty) {
    ON_CALL(mockSampleRepo, FindAll())
        .WillByDefault(Return(std::vector<Sample>{}));

    auto statuses = sut->GetInventoryStatus();

    EXPECT_TRUE(statuses.empty());
}

// 주문이 전혀 없는 경우 GetOrderSummary 모든 카운트가 0
TEST_F(MonitoringServiceFixture, GetOrderSummary_NoOrders_ShouldReturnAllZero) {
    GivenOrdersByStatus(OrderStatus::RESERVED,  {});
    GivenOrdersByStatus(OrderStatus::PRODUCING, {});
    GivenOrdersByStatus(OrderStatus::CONFIRMED, {});
    GivenOrdersByStatus(OrderStatus::RELEASE,   {});

    auto summary = sut->GetOrderSummary();

    EXPECT_EQ(summary.reservedCount,  0);
    EXPECT_EQ(summary.producingCount, 0);
    EXPECT_EQ(summary.confirmedCount, 0);
    EXPECT_EQ(summary.releaseCount,   0);
}

// 시료에 대한 재고 정보가 없는 경우 currentStock=0 으로 처리하여 DEPLETED 반환
TEST_F(MonitoringServiceFixture, GetInventoryStatus_NoInventoryRecord_ShouldBeDepleted) {
    // 시료는 존재하지만 재고 레코드가 없는 경우
    ON_CALL(mockSampleRepo, FindAll())
        .WillByDefault(Return(std::vector<Sample>{MakeSample("S-006", "ZetaSample")}));
    ON_CALL(mockInventoryRepo, FindBySampleId("S-006"))
        .WillByDefault(Return(std::nullopt));

    auto statuses = sut->GetInventoryStatus();

    ASSERT_EQ(statuses.size(), 1u);
    EXPECT_EQ(statuses[0].sampleId,     "S-006");
    EXPECT_EQ(statuses[0].currentStock, 0);
    EXPECT_EQ(statuses[0].status,       StockStatus::DEPLETED);
}

// 여러 시료가 있을 때 각각 올바른 StockStatus 를 반환
TEST_F(MonitoringServiceFixture, GetInventoryStatus_MultipleSamples_ShouldReturnCorrectStatuses) {
    std::vector<Sample> samples = {
        MakeSample("S-010", "Alpha"),
        MakeSample("S-011", "Beta"),
        MakeSample("S-012", "Gamma")
    };
    ON_CALL(mockSampleRepo, FindAll()).WillByDefault(Return(samples));

    // S-010: stock=0             → DEPLETED
    ON_CALL(mockInventoryRepo, FindBySampleId("S-010"))
        .WillByDefault(Return(std::optional<Inventory>{MakeInventory("S-010", 0, 0)}));
    // S-011: stock=5, reserved=5 → SHORTAGE (availableStock=0)
    ON_CALL(mockInventoryRepo, FindBySampleId("S-011"))
        .WillByDefault(Return(std::optional<Inventory>{MakeInventory("S-011", 5, 5)}));
    // S-012: stock=50, reserved=10 → SUFFICIENT (availableStock=40)
    ON_CALL(mockInventoryRepo, FindBySampleId("S-012"))
        .WillByDefault(Return(std::optional<Inventory>{MakeInventory("S-012", 50, 10)}));

    auto statuses = sut->GetInventoryStatus();

    ASSERT_EQ(statuses.size(), 3u);

    // 순서는 FindAll 반환 순서와 동일해야 함
    auto findStatus = [&](const std::string& sid) -> StockStatus {
        for (const auto& s : statuses) {
            if (s.sampleId == sid) return s.status;
        }
        throw std::runtime_error("not found");
    };

    EXPECT_EQ(findStatus("S-010"), StockStatus::DEPLETED);
    EXPECT_EQ(findStatus("S-011"), StockStatus::SHORTAGE);
    EXPECT_EQ(findStatus("S-012"), StockStatus::SUFFICIENT);
}
