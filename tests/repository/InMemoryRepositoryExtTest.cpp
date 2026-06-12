#include <gtest/gtest.h>
#include "../../src/model/repository/InMemoryOrderRepository.h"
#include "../../src/model/repository/InMemoryProductionRepository.h"
#include "../../src/model/repository/InMemoryInventoryRepository.h"
#include "../../src/model/domain/Order.h"
#include "../../src/model/domain/OrderStatus.h"
#include "../../src/model/domain/ProductionJob.h"
#include "../../src/model/domain/Inventory.h"

#include <chrono>
#include <string>
#include <algorithm>

// ============================================================
// InMemoryOrderRepository — FindAll() 커버리지
// ============================================================

class InMemoryOrderRepositoryExtFixture : public ::testing::Test {
protected:
    InMemoryOrderRepository repo;

    Order MakeOrder(const std::string& orderId,
                    const std::string& sampleId,
                    OrderStatus status) {
        Order o;
        o.orderId      = orderId;
        o.sampleId     = sampleId;
        o.customerName = "TestCustomer";
        o.quantity     = 5;
        o.status       = status;
        o.createdAt    = std::chrono::system_clock::now();
        return o;
    }
};

// Normal — 저장된 주문이 없을 때 FindAll()은 빈 벡터를 반환한다
TEST_F(InMemoryOrderRepositoryExtFixture, FindAll_EmptyStore_ShouldReturnEmptyVector) {
    auto result = repo.FindAll();

    EXPECT_TRUE(result.empty());
}

// Normal — 복수 주문 저장 후 FindAll()은 모든 주문을 반환한다
TEST_F(InMemoryOrderRepositoryExtFixture, FindAll_MultipleOrders_ShouldReturnAllOrders) {
    repo.Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));
    repo.Save(MakeOrder("ORD-002", "S-002", OrderStatus::CONFIRMED));
    repo.Save(MakeOrder("ORD-003", "S-001", OrderStatus::PRODUCING));

    auto result = repo.FindAll();

    EXPECT_EQ(result.size(), 3u);
}

// Normal — 단일 주문 저장 후 FindAll()이 해당 주문을 포함한다
TEST_F(InMemoryOrderRepositoryExtFixture, FindAll_SingleOrder_ShouldContainSavedOrder) {
    repo.Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));

    auto result = repo.FindAll();

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].orderId, "ORD-001");
    EXPECT_EQ(result[0].status, OrderStatus::RESERVED);
}

// Normal — Save로 상태를 갱신한 후 FindAll()이 최신 상태를 반환한다
TEST_F(InMemoryOrderRepositoryExtFixture, FindAll_AfterUpdate_ShouldReflectLatestStatus) {
    repo.Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));

    Order updated = MakeOrder("ORD-001", "S-001", OrderStatus::CONFIRMED);
    repo.Save(updated);

    auto result = repo.FindAll();

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].status, OrderStatus::CONFIRMED);
}

// Boundary — 다양한 상태의 주문이 혼재할 때 FindAll()은 모두 반환한다
TEST_F(InMemoryOrderRepositoryExtFixture, FindAll_AllStatusTypes_ShouldReturnAll) {
    repo.Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));
    repo.Save(MakeOrder("ORD-002", "S-001", OrderStatus::PRODUCING));
    repo.Save(MakeOrder("ORD-003", "S-001", OrderStatus::CONFIRMED));
    repo.Save(MakeOrder("ORD-004", "S-001", OrderStatus::RELEASE));
    repo.Save(MakeOrder("ORD-005", "S-001", OrderStatus::REJECTED));

    auto result = repo.FindAll();

    EXPECT_EQ(result.size(), 5u);
}

// ============================================================
// InMemoryProductionRepository — FindAll() + FindById nullopt
// ============================================================

class InMemoryProductionRepositoryExtFixture : public ::testing::Test {
protected:
    InMemoryProductionRepository repo;

    ProductionJob MakeJob(const std::string& productionId,
                          const std::string& orderId,
                          const std::string& sampleId) {
        ProductionJob job;
        job.productionId              = productionId;
        job.orderId                   = orderId;
        job.sampleId                  = sampleId;
        job.requiredQuantity          = 10;
        job.actualProductionQuantity  = 12;
        job.totalProductionTime       = 360.0;
        job.startTime                 = std::chrono::system_clock::now();
        job.expectedFinishTime        = job.startTime +
                                        std::chrono::minutes(360);
        return job;
    }
};

// Abnormal — 존재하지 않는 ID로 FindById()를 호출하면 nullopt를 반환한다
TEST_F(InMemoryProductionRepositoryExtFixture, FindById_NotFound_ShouldReturnNullopt) {
    auto result = repo.FindById("NONEXISTENT");

    EXPECT_FALSE(result.has_value());
}

// Abnormal — 빈 문자열 ID로 FindById()를 호출하면 nullopt를 반환한다
TEST_F(InMemoryProductionRepositoryExtFixture, FindById_EmptyId_ShouldReturnNullopt) {
    repo.Save(MakeJob("PROD-001", "ORD-001", "S-001"));

    auto result = repo.FindById("");

    EXPECT_FALSE(result.has_value());
}

// Normal — 저장된 작업이 없을 때 FindAll()은 빈 벡터를 반환한다
TEST_F(InMemoryProductionRepositoryExtFixture, FindAll_EmptyStore_ShouldReturnEmptyVector) {
    auto result = repo.FindAll();

    EXPECT_TRUE(result.empty());
}

// Normal — 복수 작업 저장 후 FindAll()은 모든 작업을 반환한다
TEST_F(InMemoryProductionRepositoryExtFixture, FindAll_MultipleJobs_ShouldReturnAllJobs) {
    repo.Save(MakeJob("PROD-001", "ORD-001", "S-001"));
    repo.Save(MakeJob("PROD-002", "ORD-002", "S-002"));
    repo.Save(MakeJob("PROD-003", "ORD-003", "S-001"));

    auto result = repo.FindAll();

    EXPECT_EQ(result.size(), 3u);
}

// Normal — 단일 작업 저장 후 FindAll()이 해당 작업을 포함한다
TEST_F(InMemoryProductionRepositoryExtFixture, FindAll_SingleJob_ShouldContainSavedJob) {
    repo.Save(MakeJob("PROD-001", "ORD-001", "S-001"));

    auto result = repo.FindAll();

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].productionId, "PROD-001");
    EXPECT_EQ(result[0].orderId, "ORD-001");
}

// Normal — Save로 작업을 갱신한 후 FindAll()이 최신 값을 반환한다
TEST_F(InMemoryProductionRepositoryExtFixture, FindAll_AfterUpdate_ShouldReflectUpdatedJob) {
    repo.Save(MakeJob("PROD-001", "ORD-001", "S-001"));

    ProductionJob updated = MakeJob("PROD-001", "ORD-001", "S-002");
    repo.Save(updated);

    auto result = repo.FindAll();

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].sampleId, "S-002");
}

// Boundary — 동일 orderId로 복수 작업 저장 시 FindAll()이 모두 반환한다
TEST_F(InMemoryProductionRepositoryExtFixture, FindAll_SameOrderId_MultipleJobs_ShouldReturnAll) {
    repo.Save(MakeJob("PROD-001", "ORD-001", "S-001"));
    repo.Save(MakeJob("PROD-002", "ORD-001", "S-002"));

    auto result = repo.FindAll();

    EXPECT_EQ(result.size(), 2u);
}

// ============================================================
// InMemoryInventoryRepository — FindBySampleId nullopt (line 15)
// ============================================================

class InMemoryInventoryRepositoryExtFixture : public ::testing::Test {
protected:
    InMemoryInventoryRepository repo;

    Inventory MakeInventory(const std::string& sampleId,
                            int currentStock,
                            int reservedStock) {
        Inventory inv;
        inv.sampleId      = sampleId;
        inv.currentStock  = currentStock;
        inv.reservedStock = reservedStock;
        return inv;
    }
};

// Abnormal — 존재하지 않는 sampleId로 FindBySampleId()를 호출하면 nullopt를 반환한다
TEST_F(InMemoryInventoryRepositoryExtFixture, FindBySampleId_NotFound_ShouldReturnNullopt) {
    auto result = repo.FindBySampleId("NONEXISTENT");

    EXPECT_FALSE(result.has_value());
}

// Abnormal — 빈 문자열 sampleId로 FindBySampleId()를 호출하면 nullopt를 반환한다
TEST_F(InMemoryInventoryRepositoryExtFixture, FindBySampleId_EmptyId_ShouldReturnNullopt) {
    repo.Save(MakeInventory("S-001", 100, 10));

    auto result = repo.FindBySampleId("");

    EXPECT_FALSE(result.has_value());
}

// Abnormal — 빈 저장소에서 FindBySampleId()를 호출하면 nullopt를 반환한다
TEST_F(InMemoryInventoryRepositoryExtFixture, FindBySampleId_EmptyStore_ShouldReturnNullopt) {
    auto result = repo.FindBySampleId("S-001");

    EXPECT_FALSE(result.has_value());
}

// Normal — 저장된 재고를 FindBySampleId()로 조회하면 정확한 값을 반환한다
TEST_F(InMemoryInventoryRepositoryExtFixture, FindBySampleId_ExistingSample_ShouldReturnCorrectInventory) {
    repo.Save(MakeInventory("S-001", 50, 10));

    auto result = repo.FindBySampleId("S-001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->currentStock, 50);
    EXPECT_EQ(result->reservedStock, 10);
    EXPECT_EQ(result->availableStock(), 40);
}

// Boundary — currentStock == reservedStock 일 때 availableStock()은 0이다
TEST_F(InMemoryInventoryRepositoryExtFixture, FindBySampleId_StockEqualReserved_AvailableShouldBeZero) {
    repo.Save(MakeInventory("S-001", 10, 10));

    auto result = repo.FindBySampleId("S-001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->availableStock(), 0);
}

// Boundary — currentStock == 0 일 때 FindBySampleId()가 정상 반환하고 availableStock()이 0이다
TEST_F(InMemoryInventoryRepositoryExtFixture, FindBySampleId_ZeroStock_ShouldReturnZeroAvailable) {
    repo.Save(MakeInventory("S-001", 0, 0));

    auto result = repo.FindBySampleId("S-001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->currentStock, 0);
    EXPECT_EQ(result->availableStock(), 0);
}

// Normal — ExistsBySampleId()가 저장된 항목에 대해 true를 반환한다
TEST_F(InMemoryInventoryRepositoryExtFixture, ExistsBySampleId_ExistingSample_ShouldReturnTrue) {
    repo.Save(MakeInventory("S-001", 20, 5));

    EXPECT_TRUE(repo.ExistsBySampleId("S-001"));
}

// Abnormal — ExistsBySampleId()가 존재하지 않는 항목에 대해 false를 반환한다
TEST_F(InMemoryInventoryRepositoryExtFixture, ExistsBySampleId_NotFound_ShouldReturnFalse) {
    EXPECT_FALSE(repo.ExistsBySampleId("NONEXISTENT"));
}
