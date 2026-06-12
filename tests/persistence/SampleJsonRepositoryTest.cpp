#include <gtest/gtest.h>
#include <cstdio>
#include <string>
#include <vector>
#include <optional>

#include "../../src/persistence/SampleJsonRepository.h"

// ============================================================
// SampleJsonRepository — 실제 파일 I/O 단위 테스트
// ============================================================

static constexpr const char* kTmpFile = "test_samples_tmp.json";

class SampleJsonRepositoryFixture : public ::testing::Test {
protected:
    std::unique_ptr<SampleJsonRepository> sut;

    void SetUp() override {
        std::remove(kTmpFile);
        sut = std::make_unique<SampleJsonRepository>(kTmpFile);
    }

    void TearDown() override {
        sut.reset();
        std::remove(kTmpFile);
    }

    // 헬퍼 — 샘플 생성
    Sample MakeSample(const std::string& id,
                      const std::string& name,
                      double avgTime = 30.0,
                      double yieldRate = 0.9) {
        Sample s;
        s.sampleId               = id;
        s.name                   = name;
        s.averageProductionTime  = avgTime;
        s.yieldRate              = yieldRate;
        return s;
    }
};

// -----------------------------------------------------------------------
// Normal — Save 호출 후 파일이 존재해야 한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, Save_ShouldPersistToFile) {
    Sample s = MakeSample("S-001", "TestSample");

    EXPECT_NO_THROW(sut->Save(s));

    // 파일이 존재하는지 확인 (다시 열 수 있으면 존재하는 것)
    std::FILE* f = std::fopen(kTmpFile, "r");
    ASSERT_NE(f, nullptr) << "JSON 파일이 생성되지 않았습니다.";
    std::fclose(f);
}

// -----------------------------------------------------------------------
// Normal — Save 후 새 인스턴스로 Load하면 동일 데이터를 복원한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, Load_ShouldRestoreFromFile) {
    Sample s = MakeSample("S-001", "AlphaSample", 45.0, 0.85);
    sut->Save(s);
    sut.reset(); // 메모리 해제

    // 동일 파일로 새 인스턴스 생성 → 자동 로드
    SampleJsonRepository repo2(kTmpFile);
    auto result = repo2.FindById("S-001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sampleId, "S-001");
    EXPECT_EQ(result->name, "AlphaSample");
    EXPECT_DOUBLE_EQ(result->averageProductionTime, 45.0);
    EXPECT_DOUBLE_EQ(result->yieldRate, 0.85);
}

// -----------------------------------------------------------------------
// Normal — Save 후 FindById가 저장된 샘플을 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindById_AfterSave_ShouldReturnSample) {
    Sample s = MakeSample("S-002", "BetaSample", 60.0, 0.92);
    sut->Save(s);

    auto result = sut->FindById("S-002");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->sampleId, "S-002");
    EXPECT_EQ(result->name, "BetaSample");
    EXPECT_DOUBLE_EQ(result->averageProductionTime, 60.0);
    EXPECT_DOUBLE_EQ(result->yieldRate, 0.92);
}

// -----------------------------------------------------------------------
// Abnormal — 존재하지 않는 ID로 FindById하면 nullopt를 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindById_NotFound_ShouldReturnNullopt) {
    auto result = sut->FindById("NONEXISTENT");

    EXPECT_FALSE(result.has_value());
}

// -----------------------------------------------------------------------
// Abnormal — 빈 문자열 ID로 FindById하면 nullopt를 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindById_EmptyId_ShouldReturnNullopt) {
    sut->Save(MakeSample("S-001", "TestSample"));

    auto result = sut->FindById("");

    EXPECT_FALSE(result.has_value());
}

// -----------------------------------------------------------------------
// Normal — 복수 저장 후 FindAll이 모든 샘플을 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindAll_ShouldReturnAllSaved) {
    sut->Save(MakeSample("S-001", "SampleA"));
    sut->Save(MakeSample("S-002", "SampleB"));
    sut->Save(MakeSample("S-003", "SampleC"));

    auto result = sut->FindAll();

    EXPECT_EQ(result.size(), 3u);
}

// -----------------------------------------------------------------------
// Boundary — 저장된 항목이 없을 때 FindAll은 빈 벡터를 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindAll_EmptyStore_ShouldReturnEmptyVector) {
    auto result = sut->FindAll();

    EXPECT_TRUE(result.empty());
}

// -----------------------------------------------------------------------
// Normal — FindAll 후 새 인스턴스로 재로드해도 동일 수의 항목을 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindAll_AfterReload_ShouldReturnAllSaved) {
    sut->Save(MakeSample("S-001", "SampleA"));
    sut->Save(MakeSample("S-002", "SampleB"));
    sut.reset();

    SampleJsonRepository repo2(kTmpFile);
    auto result = repo2.FindAll();

    EXPECT_EQ(result.size(), 2u);
}

// -----------------------------------------------------------------------
// Normal — FindByName은 name 또는 ID에 키워드가 포함된 샘플을 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindByName_ShouldReturnMatchingByIdOrName) {
    sut->Save(MakeSample("S-001", "AlphaProbe"));
    sut->Save(MakeSample("S-002", "BetaProbe"));
    sut->Save(MakeSample("S-003", "GammaSensor"));

    // 이름 키워드 "Probe"로 검색 → AlphaProbe, BetaProbe 반환
    auto byName = sut->FindByName("Probe");
    EXPECT_EQ(byName.size(), 2u);

    // ID 키워드 "S-003"으로 검색 → GammaSensor 반환
    auto byId = sut->FindByName("S-003");
    ASSERT_EQ(byId.size(), 1u);
    EXPECT_EQ(byId[0].sampleId, "S-003");
}

// -----------------------------------------------------------------------
// Abnormal — 매칭 없는 키워드로 FindByName 호출 시 빈 벡터를 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, FindByName_NoMatch_ShouldReturnEmptyVector) {
    sut->Save(MakeSample("S-001", "AlphaProbe"));

    auto result = sut->FindByName("ZZZNOMATCH");

    EXPECT_TRUE(result.empty());
}

// -----------------------------------------------------------------------
// Normal — Save 후 ExistsById는 true를 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, ExistsById_AfterSave_ShouldReturnTrue) {
    sut->Save(MakeSample("S-001", "TestSample"));

    EXPECT_TRUE(sut->ExistsById("S-001"));
}

// -----------------------------------------------------------------------
// Abnormal — 저장하지 않은 ID에 대해 ExistsById는 false를 반환한다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, ExistsById_NotSaved_ShouldReturnFalse) {
    EXPECT_FALSE(sut->ExistsById("S-999"));
}

// -----------------------------------------------------------------------
// Normal — Save로 같은 ID를 덮어쓰면 최신 값이 유지된다
// -----------------------------------------------------------------------
TEST_F(SampleJsonRepositoryFixture, Save_OverwriteSameId_ShouldRetainLatestValue) {
    sut->Save(MakeSample("S-001", "OriginalName", 30.0, 0.9));
    sut->Save(MakeSample("S-001", "UpdatedName",  60.0, 0.8));

    auto result = sut->FindById("S-001");

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "UpdatedName");
    EXPECT_DOUBLE_EQ(result->averageProductionTime, 60.0);
    EXPECT_DOUBLE_EQ(result->yieldRate, 0.8);

    // FindAll도 중복 없이 1개만 반환해야 한다
    EXPECT_EQ(sut->FindAll().size(), 1u);
}
