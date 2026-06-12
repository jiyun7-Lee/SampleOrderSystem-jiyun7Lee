#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/model/repository/IOrderRepository.h"
#include "../../src/model/repository/ISampleRepository.h"
#include "../../src/model/repository/IInventoryRepository.h"
#include "../../src/model/repository/IProductionRepository.h"
#include "../../src/model/domain/Order.h"
#include "../../src/model/domain/Sample.h"
#include "../../src/model/domain/Inventory.h"
#include "../../src/model/domain/ProductionJob.h"
#include "../../src/model/domain/OrderStatus.h"
#include "../../src/service/InventoryService.h"
#include "../../src/service/OrderService.h"
#include "../../src/util/ITimeProvider.h"
#include <stdexcept>
#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <chrono>

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;
using ::testing::AnyNumber;

// -----------------------------------------------------------------------
// Mock 클래스 정의
// -----------------------------------------------------------------------
class MockOrderRepository : public IOrderRepository {
public:
    MOCK_METHOD(void, Save, (const Order&), (override));
    MOCK_METHOD(std::optional<Order>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Order>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Order>, FindByStatus, (OrderStatus), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};

// OrderService 전용 IInventoryRepository Mock (SampleServiceTest 의 Mock 과 이름 충돌 방지)
class MockInventoryRepositoryForOrder : public IInventoryRepository {
public:
    MOCK_METHOD(void, Save, (const Inventory&), (override));
    MOCK_METHOD(std::optional<Inventory>, FindBySampleId, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsBySampleId, (const std::string&), (const, override));
};

class MockSampleRepositoryForOrder : public ISampleRepository {
public:
    MOCK_METHOD(void, Save, (const Sample&), (override));
    MOCK_METHOD(std::optional<Sample>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindByName, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};

class MockProductionRepository : public IProductionRepository {
public:
    MOCK_METHOD(void, Save, (const ProductionJob&), (override));
    MOCK_METHOD(void, DeleteById, (const std::string&), (override));
    MOCK_METHOD(std::optional<ProductionJob>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>, FindByOrderId, (const std::string&), (const, override));
};

class MockTimeProvider : public ITimeProvider {
public:
    MOCK_METHOD(std::chrono::system_clock::time_point, Now, (), (const, override));
};

// -----------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------
class OrderServiceFixture : public ::testing::Test {
protected:
    MockOrderRepository               mockOrderRepo;
    MockInventoryRepositoryForOrder   mockInventoryRepo;
    MockSampleRepositoryForOrder      mockSampleRepo;
    MockProductionRepository          mockProductionRepo;
    MockTimeProvider                  mockTime;

    std::unique_ptr<InventoryService> inventoryService;
    std::unique_ptr<OrderService>     sut;

    // 고정 기준 시각
    static std::chrono::system_clock::time_point FixedNow() {
        std::tm tm{};
        tm.tm_year = 126;  // 2026
        tm.tm_mon  = 0;    // 1월
        tm.tm_mday = 15;
        tm.tm_hour = 9;
        tm.tm_min  = 0;
        tm.tm_sec  = 0;
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    void SetUp() override {
        ON_CALL(mockTime, Now()).WillByDefault(Return(FixedNow()));

        inventoryService = std::make_unique<InventoryService>(mockInventoryRepo);
        sut = std::make_unique<OrderService>(
            mockOrderRepo,
            mockSampleRepo,
            *inventoryService,
            mockProductionRepo,
            mockTime);
    }

    // 헬퍼: 시료가 존재하도록 설정
    void GivenSampleExists(const std::string& sampleId,
                           double avgProdTime = 30.0,
                           double yieldRate   = 0.9) {
        Sample s;
        s.sampleId              = sampleId;
        s.name                  = "TestSample_" + sampleId;
        s.averageProductionTime = avgProdTime;
        s.yieldRate             = yieldRate;
        ON_CALL(mockSampleRepo, FindById(sampleId))
            .WillByDefault(Return(std::optional<Sample>{s}));
        ON_CALL(mockSampleRepo, ExistsById(sampleId))
            .WillByDefault(Return(true));
    }

    // 헬퍼: 시료가 없도록 설정
    void GivenSampleNotExists(const std::string& sampleId) {
        ON_CALL(mockSampleRepo, FindById(sampleId))
            .WillByDefault(Return(std::nullopt));
        ON_CALL(mockSampleRepo, ExistsById(sampleId))
            .WillByDefault(Return(false));
    }

    // 헬퍼: 재고 상태 설정
    void GivenInventory(const std::string& sampleId, int current, int reserved) {
        Inventory inv;
        inv.sampleId      = sampleId;
        inv.currentStock  = current;
        inv.reservedStock = reserved;
        ON_CALL(mockInventoryRepo, FindBySampleId(sampleId))
            .WillByDefault(Return(std::optional<Inventory>{inv}));
        ON_CALL(mockInventoryRepo, ExistsBySampleId(sampleId))
            .WillByDefault(Return(true));
    }

    // 헬퍼: 특정 상태의 주문이 존재하도록 설정
    void GivenOrderExists(const std::string& orderId,
                          const std::string& sampleId,
                          int quantity,
                          OrderStatus status) {
        Order o;
        o.orderId      = orderId;
        o.sampleId     = sampleId;
        o.customerName = "TestCustomer";
        o.quantity     = quantity;
        o.status       = status;
        o.createdAt    = FixedNow();
        ON_CALL(mockOrderRepo, FindById(orderId))
            .WillByDefault(Return(std::optional<Order>{o}));
        ON_CALL(mockOrderRepo, ExistsById(orderId))
            .WillByDefault(Return(true));
    }

    // 헬퍼: 주문이 없도록 설정
    void GivenOrderNotExists(const std::string& orderId) {
        ON_CALL(mockOrderRepo, FindById(orderId))
            .WillByDefault(Return(std::nullopt));
        ON_CALL(mockOrderRepo, ExistsById(orderId))
            .WillByDefault(Return(false));
    }
};

// -----------------------------------------------------------------------
// Normal Cases — PlaceOrder
// -----------------------------------------------------------------------

// 유효한 입력으로 주문 생성 시 RESERVED 상태로 저장
TEST_F(OrderServiceFixture, PlaceOrder_ValidInput_ShouldBeReserved) {
    GivenSampleExists("S-001");

    Order saved;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));
    EXPECT_CALL(mockSampleRepo, ExistsById("S-001")).Times(AnyNumber());

    sut->PlaceOrder("S-001", "홍길동", 5);

    EXPECT_EQ(saved.status, OrderStatus::RESERVED);
    EXPECT_EQ(saved.sampleId, "S-001");
    EXPECT_EQ(saved.customerName, "홍길동");
    EXPECT_EQ(saved.quantity, 5);
}

// 주문번호는 ORD-YYYYMMDD-NNNN 형식이어야 함
TEST_F(OrderServiceFixture, PlaceOrder_ValidInput_OrderIdFormatShouldMatch) {
    GivenSampleExists("S-001");

    Order saved;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));
    EXPECT_CALL(mockSampleRepo, ExistsById("S-001")).Times(AnyNumber());

    sut->PlaceOrder("S-001", "홍길동", 3);

    // 주문번호 형식: ORD-YYYYMMDD-NNNN (최소 길이 확인)
    EXPECT_FALSE(saved.orderId.empty());
    EXPECT_EQ(saved.orderId.substr(0, 4), "ORD-");
}

// -----------------------------------------------------------------------
// Normal Cases — ApproveOrder (재고 충분)
// -----------------------------------------------------------------------

// 재고 충분(availableStock >= quantity) 이면 CONFIRMED 로 전환
TEST_F(OrderServiceFixture, ApproveOrder_SufficientStock_ShouldConfirm) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::RESERVED);
    GivenInventory("S-001", 100, 0);  // availableStock=100 >= 10

    Order saved;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(AnyNumber());

    sut->ApproveOrder("ORD-001");

    EXPECT_EQ(saved.status, OrderStatus::CONFIRMED);
}

// 재고 충분 시 AllocateStock 이 호출되어 재고 예약(reservedStock 증가) 저장
TEST_F(OrderServiceFixture, ApproveOrder_SufficientStock_ShouldAllocateStock) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::RESERVED);
    GivenInventory("S-001", 100, 0);  // availableStock=100

    Inventory savedInv;
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);
    EXPECT_CALL(mockInventoryRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&savedInv));

    sut->ApproveOrder("ORD-001");

    // reservedStock = 0 + 10 = 10
    EXPECT_EQ(savedInv.reservedStock, 10);
}

// -----------------------------------------------------------------------
// Normal Cases — ApproveOrder (재고 부족)
// -----------------------------------------------------------------------

// 재고 부족(availableStock < quantity) 이면 PRODUCING 으로 전환
TEST_F(OrderServiceFixture, ApproveOrder_InsufficientStock_ShouldProducing) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::RESERVED);
    GivenInventory("S-001", 3, 0);  // availableStock=3 < 10
    GivenSampleExists("S-001");

    Order saved;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));
    EXPECT_CALL(mockProductionRepo, Save(_)).Times(1);

    sut->ApproveOrder("ORD-001");

    EXPECT_EQ(saved.status, OrderStatus::PRODUCING);
}

// 재고 부족 시 ProductionJob 이 생성되어 저장
TEST_F(OrderServiceFixture, ApproveOrder_InsufficientStock_ShouldCreateProductionJob) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::RESERVED);
    GivenInventory("S-001", 0, 0);  // availableStock=0
    GivenSampleExists("S-001", 30.0, 0.9);

    ProductionJob savedJob;
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);
    EXPECT_CALL(mockProductionRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&savedJob));

    sut->ApproveOrder("ORD-001");

    EXPECT_EQ(savedJob.orderId, "ORD-001");
    EXPECT_EQ(savedJob.sampleId, "S-001");
    EXPECT_EQ(savedJob.requiredQuantity, 10);
    // actualProductionQuantity = ceil(10 / (0.9 * 0.9)) = ceil(12.35) = 13
    EXPECT_GE(savedJob.actualProductionQuantity, 13);
    EXPECT_GT(savedJob.totalProductionTime, 0.0);
}

// -----------------------------------------------------------------------
// Normal Cases — RejectOrder
// -----------------------------------------------------------------------

// RESERVED 주문 거절 시 REJECTED 로 전환
TEST_F(OrderServiceFixture, RejectOrder_ShouldBeRejected) {
    GivenOrderExists("ORD-001", "S-001", 5, OrderStatus::RESERVED);

    Order saved;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));

    sut->RejectOrder("ORD-001");

    EXPECT_EQ(saved.status, OrderStatus::REJECTED);
}

// GetReservedOrders 는 RESERVED 상태 주문만 반환
TEST_F(OrderServiceFixture, GetReservedOrders_ShouldReturnOnlyReservedOrders) {
    Order o1;
    o1.orderId  = "ORD-001";
    o1.status   = OrderStatus::RESERVED;
    o1.quantity = 5;

    Order o2;
    o2.orderId  = "ORD-002";
    o2.status   = OrderStatus::RESERVED;
    o2.quantity = 3;

    EXPECT_CALL(mockOrderRepo, FindByStatus(OrderStatus::RESERVED))
        .Times(1)
        .WillOnce(Return(std::vector<Order>{o1, o2}));

    auto result = sut->GetReservedOrders();

    EXPECT_EQ(result.size(), 2u);
}

// -----------------------------------------------------------------------
// Normal Cases — Boundary
// -----------------------------------------------------------------------

// availableStock == quantity (경계값: 정확히 일치) 이면 CONFIRMED 전환
TEST_F(OrderServiceFixture, ApproveOrder_ExactlyMatchStock_ShouldConfirm) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::RESERVED);
    GivenInventory("S-001", 10, 0);  // availableStock=10 == quantity=10

    Order saved;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(AnyNumber());

    sut->ApproveOrder("ORD-001");

    EXPECT_EQ(saved.status, OrderStatus::CONFIRMED);
}

// availableStock == quantity - 1 (경계값: 1개 부족) 이면 PRODUCING 전환
TEST_F(OrderServiceFixture, ApproveOrder_OneShortOfStock_ShouldProducing) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::RESERVED);
    GivenInventory("S-001", 9, 0);  // availableStock=9 < 10
    GivenSampleExists("S-001");

    Order saved;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));
    EXPECT_CALL(mockProductionRepo, Save(_)).Times(1);

    sut->ApproveOrder("ORD-001");

    EXPECT_EQ(saved.status, OrderStatus::PRODUCING);
}

// -----------------------------------------------------------------------
// Abnormal Cases
// -----------------------------------------------------------------------

// 존재하지 않는 sampleId 로 주문 생성 시 예외 — Save 미호출
TEST_F(OrderServiceFixture, PlaceOrder_InvalidSampleId_ShouldThrow) {
    GivenSampleNotExists("X-999");

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->PlaceOrder("X-999", "홍길동", 5), std::runtime_error);
}

// 빈 sampleId 로 주문 생성 시 예외
TEST_F(OrderServiceFixture, PlaceOrder_EmptySampleId_ShouldThrow) {
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->PlaceOrder("", "홍길동", 5), std::invalid_argument);
}

// 빈 customerName 으로 주문 생성 시 예외
TEST_F(OrderServiceFixture, PlaceOrder_EmptyCustomerName_ShouldThrow) {
    GivenSampleExists("S-001");

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->PlaceOrder("S-001", "", 5), std::invalid_argument);
}

// quantity <= 0 으로 주문 생성 시 예외
TEST_F(OrderServiceFixture, PlaceOrder_ZeroQuantity_ShouldThrow) {
    GivenSampleExists("S-001");

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->PlaceOrder("S-001", "홍길동", 0), std::invalid_argument);
}

// RESERVED 가 아닌 주문(CONFIRMED) 을 승인 시도하면 예외 — Save 미호출
TEST_F(OrderServiceFixture, ApproveOrder_NotReserved_ShouldThrow) {
    GivenOrderExists("ORD-001", "S-001", 5, OrderStatus::CONFIRMED);

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->ApproveOrder("ORD-001"), std::logic_error);
}

// REJECTED 상태 주문을 승인 시도하면 예외
TEST_F(OrderServiceFixture, ApproveOrder_AlreadyRejected_ShouldThrow) {
    GivenOrderExists("ORD-001", "S-001", 5, OrderStatus::REJECTED);

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->ApproveOrder("ORD-001"), std::logic_error);
}

// PRODUCING 상태 주문을 거절 시도하면 예외 — Save 미호출
TEST_F(OrderServiceFixture, RejectOrder_NotReserved_ShouldThrow) {
    GivenOrderExists("ORD-001", "S-001", 5, OrderStatus::PRODUCING);

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->RejectOrder("ORD-001"), std::logic_error);
}

// 존재하지 않는 orderId 로 ApproveOrder 호출 시 예외
TEST_F(OrderServiceFixture, ApproveOrder_OrderNotFound_ShouldThrow) {
    GivenOrderNotExists("INVALID-001");

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->ApproveOrder("INVALID-001"), std::runtime_error);
}

// 존재하지 않는 orderId 로 RejectOrder 호출 시 예외
TEST_F(OrderServiceFixture, RejectOrder_OrderNotFound_ShouldThrow) {
    GivenOrderNotExists("INVALID-001");

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->RejectOrder("INVALID-001"), std::runtime_error);
}
