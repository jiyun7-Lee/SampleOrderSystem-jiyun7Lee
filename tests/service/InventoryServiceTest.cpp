#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/model/repository/IInventoryRepository.h"
#include "../../src/model/domain/Inventory.h"
#include "../../src/service/InventoryService.h"
#include <stdexcept>
#include <memory>
#include <optional>
#include <string>

using ::testing::Return;
using ::testing::_;
using ::testing::SaveArg;

// -----------------------------------------------------------------------
// Mock
// -----------------------------------------------------------------------
class MockInventoryRepository : public IInventoryRepository {
public:
    MOCK_METHOD(void, Save, (const Inventory&), (override));
    MOCK_METHOD(std::optional<Inventory>, FindBySampleId, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsBySampleId, (const std::string&), (const, override));
};

// -----------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------
class InventoryServiceFixture : public ::testing::Test {
protected:
    MockInventoryRepository mockRepo;
    std::unique_ptr<InventoryService> sut;

    void SetUp() override {
        sut = std::make_unique<InventoryService>(mockRepo);
    }

    // 헬퍼: 기본 Inventory 생성
    Inventory MakeInventory(const std::string& sampleId,
                            int currentStock,
                            int reservedStock) {
        Inventory inv;
        inv.sampleId      = sampleId;
        inv.currentStock  = currentStock;
        inv.reservedStock = reservedStock;
        return inv;
    }

    // 헬퍼: 특정 샘플의 재고가 존재한다고 설정
    void GivenInventoryExists(const std::string& sampleId,
                              int currentStock,
                              int reservedStock) {
        ON_CALL(mockRepo, FindBySampleId(sampleId))
            .WillByDefault(Return(MakeInventory(sampleId, currentStock, reservedStock)));
        ON_CALL(mockRepo, ExistsBySampleId(sampleId))
            .WillByDefault(Return(true));
    }

    // 헬퍼: 해당 샘플의 재고 항목이 없는 상태 설정
    void GivenInventoryNotExists(const std::string& sampleId) {
        ON_CALL(mockRepo, FindBySampleId(sampleId))
            .WillByDefault(Return(std::nullopt));
        ON_CALL(mockRepo, ExistsBySampleId(sampleId))
            .WillByDefault(Return(false));
    }
};

// -----------------------------------------------------------------------
// Normal Cases
// -----------------------------------------------------------------------

// availableStock = currentStock - reservedStock 반환 검증
TEST_F(InventoryServiceFixture, GetAvailableStock_ReturnsCurrentMinusReserved) {
    // currentStock=100, reservedStock=30 => availableStock=70
    GivenInventoryExists("S-001", 100, 30);

    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);

    int result = sut->GetAvailableStock("S-001");

    EXPECT_EQ(result, 70);
}

// availableStock >= qty 일 때 AllocateStock 은 reservedStock 을 증가시켜 저장
TEST_F(InventoryServiceFixture, AllocateStock_IncreasesReservedStock) {
    // currentStock=100, reservedStock=10 => availableStock=90
    GivenInventoryExists("S-001", 100, 10);

    Inventory saved;
    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);
    EXPECT_CALL(mockRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));

    sut->AllocateStock("S-001", 20);

    // reservedStock = 10 + 20 = 30, currentStock 불변
    EXPECT_EQ(saved.reservedStock, 30);
    EXPECT_EQ(saved.currentStock, 100);
}

// AddStock 은 currentStock 을 qty 만큼 증가시켜 저장
TEST_F(InventoryServiceFixture, AddStock_IncreasesCurrentStock) {
    GivenInventoryExists("S-001", 50, 0);

    Inventory saved;
    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);
    EXPECT_CALL(mockRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));

    sut->AddStock("S-001", 30);

    // currentStock = 50 + 30 = 80
    EXPECT_EQ(saved.currentStock, 80);
    EXPECT_EQ(saved.reservedStock, 0);
}

// DeductStock 은 currentStock 을 qty 만큼 감소시켜 저장
TEST_F(InventoryServiceFixture, DeductStock_DecreasesCurrentStock) {
    // currentStock=100, reservedStock=50
    GivenInventoryExists("S-001", 100, 50);

    Inventory saved;
    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);
    EXPECT_CALL(mockRepo, Save(_))
        .Times(1)
        .WillOnce(SaveArg<0>(&saved));

    sut->DeductStock("S-001", 50);

    // currentStock = 100 - 50 = 50
    EXPECT_EQ(saved.currentStock, 50);
}

// GetInventory 는 Repository 에서 Inventory 를 그대로 반환
TEST_F(InventoryServiceFixture, GetInventory_ReturnsInventoryFromRepository) {
    GivenInventoryExists("S-002", 200, 100);

    EXPECT_CALL(mockRepo, FindBySampleId("S-002")).Times(1);

    Inventory result = sut->GetInventory("S-002");

    EXPECT_EQ(result.sampleId, "S-002");
    EXPECT_EQ(result.currentStock, 200);
    EXPECT_EQ(result.reservedStock, 100);
}

// availableStock == qty (경계값: 정확히 일치) 도 AllocateStock 성공
TEST_F(InventoryServiceFixture, AllocateStock_ExactlyAvailableStock_ShouldSucceed) {
    // currentStock=10, reservedStock=0 => availableStock=10
    GivenInventoryExists("S-001", 10, 0);

    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);
    EXPECT_CALL(mockRepo, Save(_)).Times(1);

    EXPECT_NO_THROW(sut->AllocateStock("S-001", 10));
}

// -----------------------------------------------------------------------
// Abnormal Cases
// -----------------------------------------------------------------------

// qty > availableStock 이면 AllocateStock 은 예외를 던지고 Save 미호출
TEST_F(InventoryServiceFixture, AllocateStock_ExceedsAvailable_ShouldThrow) {
    // currentStock=10, reservedStock=5 => availableStock=5
    GivenInventoryExists("S-001", 10, 5);

    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);
    EXPECT_CALL(mockRepo, Save(_)).Times(0);  // 저장 없어야 함

    EXPECT_THROW(sut->AllocateStock("S-001", 6), std::runtime_error);
}

// availableStock == 0 이면 AllocateStock 은 어떤 qty > 0 에도 예외
TEST_F(InventoryServiceFixture, AllocateStock_ZeroAvailableStock_ShouldThrow) {
    // currentStock=10, reservedStock=10 => availableStock=0
    GivenInventoryExists("S-001", 10, 10);

    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);
    EXPECT_CALL(mockRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->AllocateStock("S-001", 1), std::runtime_error);
}

// qty <= 0 인 AllocateStock 요청은 invalid_argument 예외
TEST_F(InventoryServiceFixture, AllocateStock_ZeroQty_ShouldThrow) {
    GivenInventoryExists("S-001", 100, 0);

    EXPECT_CALL(mockRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->AllocateStock("S-001", 0), std::invalid_argument);
}

// qty <= 0 인 AddStock 요청은 invalid_argument 예외
TEST_F(InventoryServiceFixture, AddStock_ZeroQty_ShouldThrow) {
    GivenInventoryExists("S-001", 100, 0);

    EXPECT_CALL(mockRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->AddStock("S-001", 0), std::invalid_argument);
}

// qty <= 0 인 DeductStock 요청은 invalid_argument 예외
TEST_F(InventoryServiceFixture, DeductStock_ZeroQty_ShouldThrow) {
    GivenInventoryExists("S-001", 100, 0);

    EXPECT_CALL(mockRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->DeductStock("S-001", 0), std::invalid_argument);
}

// 존재하지 않는 sampleId 로 GetInventory 호출 시 예외
TEST_F(InventoryServiceFixture, GetInventory_NonExistingSampleId_ShouldThrow) {
    GivenInventoryNotExists("X-999");

    EXPECT_CALL(mockRepo, FindBySampleId("X-999")).Times(1);

    EXPECT_THROW(sut->GetInventory("X-999"), std::runtime_error);
}

// 존재하지 않는 sampleId 로 GetAvailableStock 호출 시 예외
TEST_F(InventoryServiceFixture, GetAvailableStock_NonExistingSampleId_ShouldThrow) {
    GivenInventoryNotExists("X-999");

    EXPECT_CALL(mockRepo, FindBySampleId("X-999")).Times(1);

    EXPECT_THROW(sut->GetAvailableStock("X-999"), std::runtime_error);
}

// DeductStock qty > currentStock 이면 예외 (재고 음수 방지)
TEST_F(InventoryServiceFixture, DeductStock_ExceedsCurrentStock_ShouldThrow) {
    GivenInventoryExists("S-001", 5, 0);

    EXPECT_CALL(mockRepo, FindBySampleId("S-001")).Times(1);
    EXPECT_CALL(mockRepo, Save(_)).Times(0);

    EXPECT_THROW(sut->DeductStock("S-001", 10), std::runtime_error);
}
