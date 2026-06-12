#include "gtest/gtest.h"
#include "../../src/persistence/InventoryJsonRepository.h"
#include "../../src/model/domain/Inventory.h"

#include <fstream>
#include <string>
#include <optional>
#include <cstdio>

static const std::string kTestFile = "test_inventory_tmp.json";

// -----------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------
class InventoryJsonRepositoryFixture : public ::testing::Test {
protected:
    std::unique_ptr<InventoryJsonRepository> sut;

    void SetUp() override {
        RemoveFile();
        sut = std::make_unique<InventoryJsonRepository>(kTestFile);
    }

    void TearDown() override {
        sut.reset();
        RemoveFile();
    }

    // ---- 헬퍼 ----
    static void RemoveFile() {
        std::remove(kTestFile.c_str());
    }

    static Inventory MakeInventory(const std::string& sampleId,
                                   int currentStock,
                                   int reservedStock = 0) {
        return Inventory{sampleId, currentStock, reservedStock};
    }

    // SUT를 새 인스턴스로 교체 → 파일에서 로드하는 효과
    void Reload() {
        sut.reset();
        sut = std::make_unique<InventoryJsonRepository>(kTestFile);
    }
};

// -----------------------------------------------------------------------
// Normal Case: Save 후 파일이 생성되고 내용이 유효해야 한다
// -----------------------------------------------------------------------
TEST_F(InventoryJsonRepositoryFixture, Save_ShouldPersistToFile) {
    Inventory inv = MakeInventory("S-001", 50, 10);

    EXPECT_NO_THROW(sut->Save(inv));

    // 파일이 실제로 생성됐는지 확인
    std::ifstream ifs(kTestFile);
    EXPECT_TRUE(ifs.good()) << "JSON 파일이 생성되어 있어야 한다";
}

// -----------------------------------------------------------------------
// Normal Case: Save 후 FindBySampleId로 동일 데이터를 반환해야 한다
// -----------------------------------------------------------------------
TEST_F(InventoryJsonRepositoryFixture, FindBySampleId_AfterSave_ShouldReturn) {
    Inventory inv = MakeInventory("S-002", 100, 20);
    sut->Save(inv);

    std::optional<Inventory> found = sut->FindBySampleId("S-002");

    ASSERT_TRUE(found.has_value()) << "저장된 재고를 조회할 수 있어야 한다";
    EXPECT_EQ(found->sampleId,      "S-002");
    EXPECT_EQ(found->currentStock,  100);
    EXPECT_EQ(found->reservedStock, 20);
}

// -----------------------------------------------------------------------
// Normal Case: Save 후 ExistsBySampleId가 true를 반환해야 한다
// -----------------------------------------------------------------------
TEST_F(InventoryJsonRepositoryFixture, ExistsBySampleId_AfterSave_ShouldReturnTrue) {
    Inventory inv = MakeInventory("S-003", 30, 0);
    sut->Save(inv);

    EXPECT_TRUE(sut->ExistsBySampleId("S-003"))
        << "저장된 sampleId는 ExistsBySampleId == true 여야 한다";
    EXPECT_FALSE(sut->ExistsBySampleId("S-999"))
        << "미등록 sampleId는 ExistsBySampleId == false 여야 한다";
}

// -----------------------------------------------------------------------
// Normal Case: 파일 로드 — 새 인스턴스 생성 시 저장된 데이터를 복원해야 한다
// -----------------------------------------------------------------------
TEST_F(InventoryJsonRepositoryFixture, Load_ShouldRestoreFromFile) {
    // 첫 번째 인스턴스에서 저장
    sut->Save(MakeInventory("S-010", 80, 5));
    sut->Save(MakeInventory("S-011", 120, 15));

    // 새 인스턴스로 재로드 (파일에서 읽기)
    Reload();

    std::optional<Inventory> inv010 = sut->FindBySampleId("S-010");
    ASSERT_TRUE(inv010.has_value()) << "재로드 후 S-010을 찾을 수 있어야 한다";
    EXPECT_EQ(inv010->currentStock,  80);
    EXPECT_EQ(inv010->reservedStock, 5);

    std::optional<Inventory> inv011 = sut->FindBySampleId("S-011");
    ASSERT_TRUE(inv011.has_value()) << "재로드 후 S-011을 찾을 수 있어야 한다";
    EXPECT_EQ(inv011->currentStock,  120);
    EXPECT_EQ(inv011->reservedStock, 15);
}

// -----------------------------------------------------------------------
// Abnormal Case: 존재하지 않는 sampleId 조회 → nullopt 반환
// -----------------------------------------------------------------------
TEST_F(InventoryJsonRepositoryFixture, FindBySampleId_NotFound_ShouldReturnNullopt) {
    std::optional<Inventory> result = sut->FindBySampleId("INVALID");

    EXPECT_FALSE(result.has_value()) << "미등록 sampleId 조회 시 nullopt 여야 한다";
}

// -----------------------------------------------------------------------
// Abnormal Case: 동일 sampleId Save 두 번 → 마지막 값으로 덮어써야 한다
// -----------------------------------------------------------------------
TEST_F(InventoryJsonRepositoryFixture, Save_DuplicateSampleId_ShouldOverwrite) {
    sut->Save(MakeInventory("S-020", 10, 0));
    sut->Save(MakeInventory("S-020", 99, 5));  // 덮어쓰기

    std::optional<Inventory> found = sut->FindBySampleId("S-020");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->currentStock,  99) << "두 번째 Save 값으로 덮어써야 한다";
    EXPECT_EQ(found->reservedStock, 5);
}

// -----------------------------------------------------------------------
// Boundary Case: currentStock = 0, reservedStock = 0 저장 후 조회
// -----------------------------------------------------------------------
TEST_F(InventoryJsonRepositoryFixture, Save_ZeroStocks_ShouldPersistCorrectly) {
    sut->Save(MakeInventory("S-030", 0, 0));

    std::optional<Inventory> found = sut->FindBySampleId("S-030");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->currentStock,  0);
    EXPECT_EQ(found->reservedStock, 0);
    EXPECT_EQ(found->availableStock(), 0);
}
