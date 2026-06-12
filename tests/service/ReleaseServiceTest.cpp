#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/model/repository/IOrderRepository.h"
#include "../../src/model/repository/IInventoryRepository.h"
#include "../../src/model/domain/Order.h"
#include "../../src/model/domain/Inventory.h"
#include "../../src/model/domain/OrderStatus.h"
#include "../../src/service/InventoryService.h"
#include "../../src/service/ReleaseService.h"
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
// Mock 클래스 정의 — ReleaseService 전용 (다른 Test 파일의 Mock 과 이름 충돌 방지)
// -----------------------------------------------------------------------
class MockOrderRepositoryForRelease : public IOrderRepository {
public:
    MOCK_METHOD(void, Save, (const Order&), (override));
    MOCK_METHOD(std::optional<Order>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Order>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Order>, FindByStatus, (OrderStatus), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};

class MockInventoryRepositoryForRelease : public IInventoryRepository {
public:
    MOCK_METHOD(void, Save, (const Inventory&), (override));
    MOCK_METHOD(std::optional<Inventory>, FindBySampleId, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsBySampleId, (const std::string&), (const, override));
};

// -----------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------
class ReleaseServiceFixture : public ::testing::Test {
protected:
    MockOrderRepositoryForRelease      mockOrderRepo;
    MockInventoryRepositoryForRelease  mockInventoryRepo;

    std::unique_ptr<InventoryService> inventoryService;
    std::unique_ptr<ReleaseService>   sut;

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
        inventoryService = std::make_unique<InventoryService>(mockInventoryRepo);
        sut = std::make_unique<ReleaseService>(mockOrderRepo, *inventoryService);
    }

    // 헬퍼: 지정 상태의 주문 생성
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
        o.createdAt    = FixedNow();
        return o;
    }

    // 헬퍼: 특정 상태의 주문이 존재하도록 설정
    void GivenOrderExists(const std::string& orderId,
                          const std::string& sampleId,
                          int quantity,
                          OrderStatus status) {
        ON_CALL(mockOrderRepo, FindById(orderId))
            .WillByDefault(Return(MakeOrder(orderId, sampleId, quantity, status)));
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

    // 헬퍼: 재고 상태 설정
    void GivenInventory(const std::string& sampleId, int currentStock, int reservedStock) {
        Inventory inv;
        inv.sampleId      = sampleId;
        inv.currentStock  = currentStock;
        inv.reservedStock = reservedStock;
        ON_CALL(mockInventoryRepo, FindBySampleId(sampleId))
            .WillByDefault(Return(inv));
        ON_CALL(mockInventoryRepo, ExistsBySampleId(sampleId))
            .WillByDefault(Return(true));
    }
};

// -----------------------------------------------------------------------
// Normal Cases
// -----------------------------------------------------------------------

// GetConfirmedOrders 는 CONFIRMED 상태 주문만 반환해야 한다
TEST_F(ReleaseServiceFixture, GetConfirmedOrders_ShouldReturnOnlyConfirmed) {
    Order o1 = MakeOrder("ORD-001", "S-001", 10, OrderStatus::CONFIRMED);
    Order o2 = MakeOrder("ORD-002", "S-002",  5, OrderStatus::CONFIRMED);

    EXPECT_CALL(mockOrderRepo, FindByStatus(OrderStatus::CONFIRMED))
        .Times(1)
        .WillOnce(Return(std::vector<Order>{o1, o2}));

    auto result = sut->GetConfirmedOrders();

    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0].orderId, "ORD-001");
    EXPECT_EQ(result[1].orderId, "ORD-002");
    for (const auto& o : result) {
        EXPECT_EQ(o.status, OrderStatus::CONFIRMED);
    }
}

// CONFIRMED 주문을 출고 처리하면 상태가 RELEASE 로 변경되어 저장돼야 한다
TEST_F(ReleaseServiceFixture, ReleaseOrder_ShouldChangeStatusToRelease) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::CONFIRMED);
    GivenInventory("S-001", 50, 10);  // currentStock=50, reservedStock=10

    Order savedOrder;
    EXPECT_CALL(mockOrderRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&savedOrder));
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(AnyNumber());

    sut->ReleaseOrder("ORD-001");

    EXPECT_EQ(savedOrder.status, OrderStatus::RELEASE);
    EXPECT_EQ(savedOrder.orderId, "ORD-001");
}

// ReleaseOrder 는 currentStock 을 주문 수량만큼 차감(DeductStock)해야 한다
TEST_F(ReleaseServiceFixture, ReleaseOrder_ShouldDeductStock) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::CONFIRMED);
    // currentStock=50, reservedStock=10 — DeductStock 호출 후 currentStock=40
    GivenInventory("S-001", 50, 10);

    // SaveArg 로 실제 저장된 Inventory 캡처
    // DeductStock → Save 1회, DeallocateStock → Save 1회 (총 2회)
    std::vector<Inventory> savedInventories;
    EXPECT_CALL(mockInventoryRepo, Save(_))
        .Times(AnyNumber())
        .WillRepeatedly([&savedInventories](const Inventory& inv) {
            savedInventories.push_back(inv);
        });
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);

    sut->ReleaseOrder("ORD-001");

    // 최소 1회 Save 에서 currentStock 이 차감되었는지 검증
    bool deducted = false;
    for (const auto& inv : savedInventories) {
        if (inv.currentStock == 40) {  // 50 - 10 = 40
            deducted = true;
            break;
        }
    }
    EXPECT_TRUE(deducted) << "currentStock should have been reduced from 50 to 40";
}

// ReleaseOrder 는 reservedStock 을 주문 수량만큼 감소(DeallocateStock)해야 한다
TEST_F(ReleaseServiceFixture, ReleaseOrder_ShouldDeallocateReservedStock) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::CONFIRMED);
    // currentStock=50, reservedStock=10
    GivenInventory("S-001", 50, 10);

    std::vector<Inventory> savedInventories;
    EXPECT_CALL(mockInventoryRepo, Save(_))
        .Times(AnyNumber())
        .WillRepeatedly([&savedInventories](const Inventory& inv) {
            savedInventories.push_back(inv);
        });
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);

    sut->ReleaseOrder("ORD-001");

    // 최종 상태에서 reservedStock 이 0 으로 감소했는지 검증 (10 - 10 = 0)
    bool deallocated = false;
    for (const auto& inv : savedInventories) {
        if (inv.reservedStock == 0) {
            deallocated = true;
            break;
        }
    }
    EXPECT_TRUE(deallocated) << "reservedStock should have been reduced from 10 to 0";
}

// -----------------------------------------------------------------------
// Abnormal Cases
// -----------------------------------------------------------------------

// CONFIRMED 가 아닌 상태(RESERVED)의 주문을 출고 처리하면 예외 — Save 미호출
TEST_F(ReleaseServiceFixture, ReleaseOrder_NotConfirmed_ShouldThrow) {
    GivenOrderExists("ORD-001", "S-001", 10, OrderStatus::RESERVED);

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->ReleaseOrder("ORD-001"), std::logic_error);
}

// 존재하지 않는 orderId 로 출고 처리하면 예외 — Save 미호출
TEST_F(ReleaseServiceFixture, ReleaseOrder_NotFound_ShouldThrow) {
    GivenOrderNotExists("INVALID-001");

    EXPECT_CALL(mockOrderRepo, Save(_)).Times(0);
    EXPECT_CALL(mockInventoryRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->ReleaseOrder("INVALID-001"), std::runtime_error);
}
