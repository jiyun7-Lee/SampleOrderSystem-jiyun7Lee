#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>
#include <optional>
#include <chrono>

#include "../../src/persistence/OrderJsonRepository.h"

// ============================================================
// OrderJsonRepository — 실제 파일 I/O 단위 테스트
// ============================================================

static constexpr const char* kTmpOrderFile = "test_orders_tmp.json";

class OrderJsonRepositoryFixture : public ::testing::Test {
protected:
    std::unique_ptr<OrderJsonRepository> sut;

    void SetUp() override {
        std::remove(kTmpOrderFile);
        sut = std::make_unique<OrderJsonRepository>(kTmpOrderFile);
    }

    void TearDown() override {
        sut.reset();
        std::remove(kTmpOrderFile);
    }

    // 헬퍼 — 주문 생성
    Order MakeOrder(const std::string& orderId,
                    const std::string& sampleId,
                    OrderStatus status,
                    int quantity = 5,
                    const std::string& customerName = "TestCustomer") {
        Order o;
        o.orderId      = orderId;
        o.sampleId     = sampleId;
        o.customerName = customerName;
        o.quantity     = quantity;
        o.status       = status;
        o.createdAt    = std::chrono::system_clock::now();
        return o;
    }
};

// -----------------------------------------------------------------------
// Normal — Save 호출 후 파일이 생성되어야 한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, Save_ShouldPersistToFile) {
    Order o = MakeOrder("ORD-20260612-0001", "S-001", OrderStatus::RESERVED);

    EXPECT_NO_THROW(sut->Save(o));

    std::FILE* f = std::fopen(kTmpOrderFile, "r");
    ASSERT_NE(f, nullptr) << "JSON 파일이 생성되지 않았습니다.";
    std::fclose(f);
}

// -----------------------------------------------------------------------
// Normal — Save 후 FindById가 저장된 주문을 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindById_AfterSave_ShouldReturnOrder) {
    Order o = MakeOrder("ORD-20260612-0001", "S-001", OrderStatus::RESERVED, 10, "홍길동");
    sut->Save(o);

    auto result = sut->FindById("ORD-20260612-0001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-20260612-0001");
    EXPECT_EQ(result->sampleId, "S-001");
    EXPECT_EQ(result->customerName, "홍길동");
    EXPECT_EQ(result->quantity, 10);
    EXPECT_EQ(result->status, OrderStatus::RESERVED);
}

// -----------------------------------------------------------------------
// Abnormal — 존재하지 않는 ID로 FindById 호출 시 nullopt를 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindById_NotFound_ShouldReturnNullopt) {
    auto result = sut->FindById("NONEXISTENT");

    EXPECT_FALSE(result.has_value());
}

// -----------------------------------------------------------------------
// Abnormal — 빈 문자열 ID로 FindById 호출 시 nullopt를 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindById_EmptyId_ShouldReturnNullopt) {
    sut->Save(MakeOrder("ORD-20260612-0001", "S-001", OrderStatus::RESERVED));

    auto result = sut->FindById("");

    EXPECT_FALSE(result.has_value());
}

// -----------------------------------------------------------------------
// Normal — Save 후 새 인스턴스로 Load하면 동일 주문을 복원한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, Load_ShouldRestoreFromFile) {
    Order o = MakeOrder("ORD-20260612-0001", "S-002", OrderStatus::CONFIRMED, 7, "이순신");
    sut->Save(o);
    sut.reset();

    OrderJsonRepository repo2(kTmpOrderFile);
    auto result = repo2.FindById("ORD-20260612-0001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-20260612-0001");
    EXPECT_EQ(result->sampleId, "S-002");
    EXPECT_EQ(result->customerName, "이순신");
    EXPECT_EQ(result->quantity, 7);
    EXPECT_EQ(result->status, OrderStatus::CONFIRMED);
}

// -----------------------------------------------------------------------
// Normal — FindByStatus는 지정 상태의 주문만 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindByStatus_ShouldReturnMatchingOrders) {
    sut->Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));
    sut->Save(MakeOrder("ORD-002", "S-001", OrderStatus::CONFIRMED));
    sut->Save(MakeOrder("ORD-003", "S-002", OrderStatus::RESERVED));
    sut->Save(MakeOrder("ORD-004", "S-002", OrderStatus::PRODUCING));

    auto reserved = sut->FindByStatus(OrderStatus::RESERVED);
    EXPECT_EQ(reserved.size(), 2u);

    auto confirmed = sut->FindByStatus(OrderStatus::CONFIRMED);
    ASSERT_EQ(confirmed.size(), 1u);
    EXPECT_EQ(confirmed[0].orderId, "ORD-002");

    auto producing = sut->FindByStatus(OrderStatus::PRODUCING);
    ASSERT_EQ(producing.size(), 1u);
    EXPECT_EQ(producing[0].orderId, "ORD-004");
}

// -----------------------------------------------------------------------
// Boundary — 해당 상태의 주문이 없을 때 FindByStatus는 빈 벡터를 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindByStatus_NoMatch_ShouldReturnEmptyVector) {
    sut->Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));

    auto result = sut->FindByStatus(OrderStatus::RELEASE);

    EXPECT_TRUE(result.empty());
}

// -----------------------------------------------------------------------
// Normal — 복수 저장 후 FindAll이 모든 주문을 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindAll_ShouldReturnAllSaved) {
    sut->Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));
    sut->Save(MakeOrder("ORD-002", "S-001", OrderStatus::CONFIRMED));
    sut->Save(MakeOrder("ORD-003", "S-002", OrderStatus::PRODUCING));

    auto result = sut->FindAll();

    EXPECT_EQ(result.size(), 3u);
}

// -----------------------------------------------------------------------
// Boundary — 저장된 항목이 없을 때 FindAll은 빈 벡터를 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindAll_EmptyStore_ShouldReturnEmptyVector) {
    auto result = sut->FindAll();

    EXPECT_TRUE(result.empty());
}

// -----------------------------------------------------------------------
// Normal — FindAll 후 새 인스턴스로 재로드해도 동일 수의 항목을 반환한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindAll_AfterReload_ShouldReturnAllSaved) {
    sut->Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));
    sut->Save(MakeOrder("ORD-002", "S-002", OrderStatus::CONFIRMED));
    sut.reset();

    OrderJsonRepository repo2(kTmpOrderFile);
    auto result = repo2.FindAll();

    EXPECT_EQ(result.size(), 2u);
}

// -----------------------------------------------------------------------
// Normal — Save로 같은 ID 주문을 덮어쓰면 최신 상태가 유지된다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, Save_OverwriteSameId_ShouldRetainLatestStatus) {
    sut->Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));
    sut->Save(MakeOrder("ORD-001", "S-001", OrderStatus::CONFIRMED));

    auto result = sut->FindById("ORD-001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->status, OrderStatus::CONFIRMED);

    // FindAll도 중복 없이 1개만 반환해야 한다
    EXPECT_EQ(sut->FindAll().size(), 1u);
}

// -----------------------------------------------------------------------
// Normal — 모든 상태 타입을 저장·복원해도 상태값이 정확히 일치한다
// -----------------------------------------------------------------------
TEST_F(OrderJsonRepositoryFixture, FindByStatus_AllStatusTypes_ShouldBeRestoredCorrectly) {
    sut->Save(MakeOrder("ORD-001", "S-001", OrderStatus::RESERVED));
    sut->Save(MakeOrder("ORD-002", "S-001", OrderStatus::PRODUCING));
    sut->Save(MakeOrder("ORD-003", "S-001", OrderStatus::CONFIRMED));
    sut->Save(MakeOrder("ORD-004", "S-001", OrderStatus::RELEASE));
    sut->Save(MakeOrder("ORD-005", "S-001", OrderStatus::REJECTED));

    // 새 인스턴스로 재로드
    sut.reset();
    OrderJsonRepository repo2(kTmpOrderFile);

    EXPECT_EQ(repo2.FindByStatus(OrderStatus::RESERVED).size(),  1u);
    EXPECT_EQ(repo2.FindByStatus(OrderStatus::PRODUCING).size(),  1u);
    EXPECT_EQ(repo2.FindByStatus(OrderStatus::CONFIRMED).size(),  1u);
    EXPECT_EQ(repo2.FindByStatus(OrderStatus::RELEASE).size(),    1u);
    EXPECT_EQ(repo2.FindByStatus(OrderStatus::REJECTED).size(),   1u);
}
