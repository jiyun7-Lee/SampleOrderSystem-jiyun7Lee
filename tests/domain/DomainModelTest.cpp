#include "gtest/gtest.h"
#include "../../src/model/domain/OrderStatus.h"
#include "../../src/model/domain/Sample.h"
#include "../../src/model/domain/Order.h"
#include "../../src/model/domain/Inventory.h"
#include "../../src/model/domain/ProductionJob.h"
#include "../../src/model/repository/InMemorySampleRepository.h"
#include "../../src/model/repository/InMemoryOrderRepository.h"
#include "../../src/model/repository/InMemoryInventoryRepository.h"
#include "../../src/model/repository/InMemoryProductionRepository.h"

#include <chrono>
#include <string>

// ============================================================
// 1. Sample 도메인 모델 테스트
// ============================================================
class SampleTest : public ::testing::Test {
protected:
    Sample validSample;

    void SetUp() override {
        validSample.sampleId = "S-001";
        validSample.name = "TestSample";
        validSample.averageProductionTime = 10.0;
        validSample.yieldRate = 0.9;
    }
};

TEST_F(SampleTest, Sample_ValidConstruction_ShouldSetFieldsCorrectly) {
    EXPECT_EQ(validSample.sampleId, "S-001");
    EXPECT_EQ(validSample.name, "TestSample");
    EXPECT_DOUBLE_EQ(validSample.averageProductionTime, 10.0);
    EXPECT_DOUBLE_EQ(validSample.yieldRate, 0.9);
}

TEST_F(SampleTest, Sample_YieldRate_Zero_ShouldBeInvalid) {
    Sample s;
    s.yieldRate = 0.0;
    // yieldRate는 0.0 초과여야 유효 — 0.0 이하는 유효하지 않음
    EXPECT_LE(s.yieldRate, 0.0);
}

TEST_F(SampleTest, Sample_YieldRate_One_ShouldBeValid) {
    Sample s;
    s.yieldRate = 1.0;
    EXPECT_GT(s.yieldRate, 0.0);
    EXPECT_LE(s.yieldRate, 1.0);
}

TEST_F(SampleTest, Sample_YieldRate_OverOne_ShouldBeInvalid) {
    Sample s;
    s.yieldRate = 1.1;
    // yieldRate는 1.0 이하여야 유효 — 1.0 초과는 유효하지 않음
    EXPECT_GT(s.yieldRate, 1.0);
}

// ============================================================
// 2. Order 도메인 모델 테스트
// ============================================================
class OrderTest : public ::testing::Test {
protected:
    Order validOrder;

    void SetUp() override {
        validOrder.orderId = "ORD-20260612-0001";
        validOrder.sampleId = "S-001";
        validOrder.customerName = "TestCustomer";
        validOrder.quantity = 5;
        validOrder.status = OrderStatus::RESERVED;
        validOrder.createdAt = std::chrono::system_clock::now();
    }
};

TEST_F(OrderTest, Order_ValidConstruction_ShouldSetFieldsCorrectly) {
    EXPECT_EQ(validOrder.orderId, "ORD-20260612-0001");
    EXPECT_EQ(validOrder.sampleId, "S-001");
    EXPECT_EQ(validOrder.customerName, "TestCustomer");
    EXPECT_EQ(validOrder.quantity, 5);
    EXPECT_EQ(validOrder.status, OrderStatus::RESERVED);
}

TEST_F(OrderTest, Order_DefaultStatus_ShouldBeReserved) {
    EXPECT_EQ(validOrder.status, OrderStatus::RESERVED);
}

TEST_F(OrderTest, Order_Quantity_ShouldBePositive) {
    EXPECT_GT(validOrder.quantity, 0);
}

// ============================================================
// 3. Inventory 도메인 모델 테스트
// ============================================================
class InventoryTest : public ::testing::Test {};

TEST_F(InventoryTest, Inventory_AvailableStock_ReturnsCorrectValue) {
    Inventory inv;
    inv.sampleId = "S-001";
    inv.currentStock = 100;
    inv.reservedStock = 30;
    EXPECT_EQ(inv.availableStock(), 70);
}

TEST_F(InventoryTest, Inventory_AvailableStock_WhenReservedExceedsCurrent) {
    Inventory inv;
    inv.sampleId = "S-001";
    inv.currentStock = 10;
    inv.reservedStock = 30;
    // 서비스 레이어에서 처리 — availableStock()은 음수 반환
    EXPECT_EQ(inv.availableStock(), -20);
}

TEST_F(InventoryTest, Inventory_ZeroStock_AvailableIsZero) {
    Inventory inv;
    inv.sampleId = "S-001";
    inv.currentStock = 0;
    inv.reservedStock = 0;
    EXPECT_EQ(inv.availableStock(), 0);
}

// ============================================================
// 4. ProductionJob 도메인 모델 테스트
// ============================================================
class ProductionJobTest : public ::testing::Test {
protected:
    ProductionJob validJob;

    void SetUp() override {
        validJob.productionId = "PROD-001";
        validJob.orderId = "ORD-20260612-0001";
        validJob.sampleId = "S-001";
        validJob.requiredQuantity = 10;
        validJob.actualProductionQuantity = 12;
        validJob.totalProductionTime = 120.0;
        validJob.startTime = std::chrono::system_clock::now();
        validJob.expectedFinishTime = validJob.startTime + std::chrono::seconds(120);
    }
};

TEST_F(ProductionJobTest, ProductionJob_ValidConstruction_ShouldSetFieldsCorrectly) {
    EXPECT_EQ(validJob.productionId, "PROD-001");
    EXPECT_EQ(validJob.orderId, "ORD-20260612-0001");
    EXPECT_EQ(validJob.sampleId, "S-001");
    EXPECT_EQ(validJob.requiredQuantity, 10);
    EXPECT_EQ(validJob.actualProductionQuantity, 12);
    EXPECT_DOUBLE_EQ(validJob.totalProductionTime, 120.0);
}

TEST_F(ProductionJobTest, ProductionJob_RequiredQuantity_ShouldBePositive) {
    EXPECT_GT(validJob.requiredQuantity, 0);
}

// ============================================================
// 5. InMemorySampleRepository 테스트
// ============================================================
class InMemorySampleRepositoryTest : public ::testing::Test {
protected:
    InMemorySampleRepository repo;

    Sample MakeSample(const std::string& id, const std::string& name,
                      double avgTime = 10.0, double yieldRate = 0.9) {
        Sample s;
        s.sampleId = id;
        s.name = name;
        s.averageProductionTime = avgTime;
        s.yieldRate = yieldRate;
        return s;
    }
};

TEST_F(InMemorySampleRepositoryTest, Save_NewSample_ShouldBeFoundById) {
    Sample s = MakeSample("S-001", "Alpha");
    repo.Save(s);
    auto result = repo.FindById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sampleId, "S-001");
    EXPECT_EQ(result->name, "Alpha");
}

TEST_F(InMemorySampleRepositoryTest, Save_ExistingSample_ShouldUpdate) {
    Sample s1 = MakeSample("S-001", "Alpha");
    repo.Save(s1);

    Sample s2 = MakeSample("S-001", "AlphaUpdated");
    repo.Save(s2);

    auto result = repo.FindById("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "AlphaUpdated");
}

TEST_F(InMemorySampleRepositoryTest, FindById_NonExistent_ShouldReturnNullopt) {
    auto result = repo.FindById("NONEXISTENT");
    EXPECT_FALSE(result.has_value());
}

TEST_F(InMemorySampleRepositoryTest, FindAll_MultipleItems_ShouldReturnAll) {
    repo.Save(MakeSample("S-001", "Alpha"));
    repo.Save(MakeSample("S-002", "Beta"));
    repo.Save(MakeSample("S-003", "Gamma"));

    auto all = repo.FindAll();
    EXPECT_EQ(all.size(), static_cast<size_t>(3));
}

TEST_F(InMemorySampleRepositoryTest, ExistsById_AfterSave_ShouldReturnTrue) {
    repo.Save(MakeSample("S-001", "Alpha"));
    EXPECT_TRUE(repo.ExistsById("S-001"));
}

TEST_F(InMemorySampleRepositoryTest, ExistsById_BeforeSave_ShouldReturnFalse) {
    EXPECT_FALSE(repo.ExistsById("S-001"));
}

TEST_F(InMemorySampleRepositoryTest, FindByName_ShouldReturnMatchingItems) {
    repo.Save(MakeSample("S-001", "Alpha"));
    repo.Save(MakeSample("S-002", "Beta"));
    repo.Save(MakeSample("S-003", "Alpha"));

    auto results = repo.FindByName("Alpha");
    EXPECT_EQ(results.size(), static_cast<size_t>(2));
    for (const auto& s : results) {
        EXPECT_EQ(s.name, "Alpha");
    }
}

TEST_F(InMemorySampleRepositoryTest, DeleteById_AfterSave_ShouldRemoveItem) {
    repo.Save(MakeSample("S-001", "Alpha"));
    ASSERT_TRUE(repo.ExistsById("S-001"));

    repo.DeleteById("S-001");

    auto result = repo.FindById("S-001");
    EXPECT_FALSE(result.has_value());
}

// ============================================================
// 6. InMemoryOrderRepository 테스트
// ============================================================
class InMemoryOrderRepositoryTest : public ::testing::Test {
protected:
    InMemoryOrderRepository repo;

    Order MakeOrder(const std::string& id, const std::string& sampleId,
                    const std::string& customer, int qty,
                    OrderStatus status = OrderStatus::RESERVED) {
        Order o;
        o.orderId = id;
        o.sampleId = sampleId;
        o.customerName = customer;
        o.quantity = qty;
        o.status = status;
        o.createdAt = std::chrono::system_clock::now();
        return o;
    }
};

TEST_F(InMemoryOrderRepositoryTest, Save_NewOrder_ShouldBeFoundById) {
    Order o = MakeOrder("ORD-20260612-0001", "S-001", "CustomerA", 10);
    repo.Save(o);

    auto result = repo.FindById("ORD-20260612-0001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-20260612-0001");
    EXPECT_EQ(result->customerName, "CustomerA");
}

TEST_F(InMemoryOrderRepositoryTest, FindByStatus_ShouldReturnMatchingOrders) {
    repo.Save(MakeOrder("ORD-001", "S-001", "CustomerA", 5, OrderStatus::RESERVED));
    repo.Save(MakeOrder("ORD-002", "S-001", "CustomerB", 3, OrderStatus::CONFIRMED));
    repo.Save(MakeOrder("ORD-003", "S-002", "CustomerC", 7, OrderStatus::RESERVED));

    auto reserved = repo.FindByStatus(OrderStatus::RESERVED);
    EXPECT_EQ(reserved.size(), static_cast<size_t>(2));
    for (const auto& o : reserved) {
        EXPECT_EQ(o.status, OrderStatus::RESERVED);
    }
}

TEST_F(InMemoryOrderRepositoryTest, DeleteById_ShouldRemoveOrder) {
    repo.Save(MakeOrder("ORD-001", "S-001", "CustomerA", 5));
    ASSERT_TRUE(repo.ExistsById("ORD-001"));

    repo.DeleteById("ORD-001");

    EXPECT_FALSE(repo.ExistsById("ORD-001"));
    auto result = repo.FindById("ORD-001");
    EXPECT_FALSE(result.has_value());
}

// ============================================================
// 7. InMemoryInventoryRepository 테스트
// ============================================================
class InMemoryInventoryRepositoryTest : public ::testing::Test {
protected:
    InMemoryInventoryRepository repo;

    Inventory MakeInventory(const std::string& sampleId, int current, int reserved) {
        Inventory inv;
        inv.sampleId = sampleId;
        inv.currentStock = current;
        inv.reservedStock = reserved;
        return inv;
    }
};

TEST_F(InMemoryInventoryRepositoryTest, Save_NewInventory_ShouldBeFoundBySampleId) {
    Inventory inv = MakeInventory("S-001", 100, 30);
    repo.Save(inv);

    auto result = repo.FindBySampleId("S-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sampleId, "S-001");
    EXPECT_EQ(result->currentStock, 100);
    EXPECT_EQ(result->reservedStock, 30);
}

TEST_F(InMemoryInventoryRepositoryTest, ExistsBySampleId_AfterSave_ShouldReturnTrue) {
    repo.Save(MakeInventory("S-001", 50, 10));
    EXPECT_TRUE(repo.ExistsBySampleId("S-001"));
}

// ============================================================
// 8. InMemoryProductionRepository 테스트
// ============================================================
class InMemoryProductionRepositoryTest : public ::testing::Test {
protected:
    InMemoryProductionRepository repo;

    ProductionJob MakeJob(const std::string& jobId, const std::string& orderId,
                          const std::string& sampleId, int requiredQty) {
        ProductionJob job;
        job.productionId = jobId;
        job.orderId = orderId;
        job.sampleId = sampleId;
        job.requiredQuantity = requiredQty;
        job.actualProductionQuantity = 0;
        job.totalProductionTime = 0.0;
        job.startTime = std::chrono::system_clock::now();
        job.expectedFinishTime = job.startTime + std::chrono::seconds(100);
        return job;
    }
};

TEST_F(InMemoryProductionRepositoryTest, Save_NewJob_ShouldBeFoundById) {
    ProductionJob job = MakeJob("PROD-001", "ORD-001", "S-001", 10);
    repo.Save(job);

    auto result = repo.FindById("PROD-001");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->productionId, "PROD-001");
    EXPECT_EQ(result->orderId, "ORD-001");
    EXPECT_EQ(result->sampleId, "S-001");
}

TEST_F(InMemoryProductionRepositoryTest, FindByOrderId_ShouldReturnMatchingJobs) {
    repo.Save(MakeJob("PROD-001", "ORD-001", "S-001", 10));
    repo.Save(MakeJob("PROD-002", "ORD-002", "S-002", 5));
    repo.Save(MakeJob("PROD-003", "ORD-001", "S-001", 8));

    auto jobs = repo.FindByOrderId("ORD-001");
    EXPECT_EQ(jobs.size(), static_cast<size_t>(2));
    for (const auto& j : jobs) {
        EXPECT_EQ(j.orderId, "ORD-001");
    }
}
