#include "gtest/gtest.h"
#include "../../src/persistence/ProductionJobJsonRepository.h"
#include "../../src/model/domain/ProductionJob.h"

#include <fstream>
#include <string>
#include <optional>
#include <vector>
#include <chrono>
#include <cstdio>

static const std::string kTestFile = "test_production_tmp.json";

// -----------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------
class ProductionJobJsonRepositoryFixture : public ::testing::Test {
protected:
    std::unique_ptr<ProductionJobJsonRepository> sut;

    // 고정 기준 시각 (재현 가능한 time_point)
    static std::chrono::system_clock::time_point BaseTime() {
        std::tm tm{};
        tm.tm_year = 126; // 2026
        tm.tm_mon  = 0;   // January
        tm.tm_mday = 15;
        tm.tm_hour = 9;
        tm.tm_min  = 0;
        tm.tm_sec  = 0;
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    static std::chrono::system_clock::time_point After(double minutes) {
        return BaseTime() +
               std::chrono::seconds(static_cast<long long>(minutes * 60));
    }

    void SetUp() override {
        RemoveFile();
        sut = std::make_unique<ProductionJobJsonRepository>(kTestFile);
    }

    void TearDown() override {
        sut.reset();
        RemoveFile();
    }

    // ---- 헬퍼 ----
    static void RemoveFile() {
        std::remove(kTestFile.c_str());
    }

    // SUT를 새 인스턴스로 교체 → 파일에서 로드하는 효과
    void Reload() {
        sut.reset();
        sut = std::make_unique<ProductionJobJsonRepository>(kTestFile);
    }

    static ProductionJob MakeJob(const std::string& productionId,
                                 const std::string& orderId,
                                 const std::string& sampleId,
                                 int requiredQty      = 10,
                                 int actualQty        = 12,
                                 double totalTime     = 360.0,
                                 double finishOffset  = 360.0) {
        ProductionJob job;
        job.productionId             = productionId;
        job.orderId                  = orderId;
        job.sampleId                 = sampleId;
        job.requiredQuantity         = requiredQty;
        job.actualProductionQuantity = actualQty;
        job.totalProductionTime      = totalTime;
        job.startTime                = BaseTime();
        job.expectedFinishTime       = After(finishOffset);
        return job;
    }
};

// -----------------------------------------------------------------------
// Normal Case: Save 후 파일이 생성되어야 한다
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, Save_ShouldPersistToFile) {
    ProductionJob job = MakeJob("PJ-001", "ORD-20260115-0001", "S-001");

    EXPECT_NO_THROW(sut->Save(job));

    std::ifstream ifs(kTestFile);
    EXPECT_TRUE(ifs.good()) << "JSON 파일이 생성되어 있어야 한다";
}

// -----------------------------------------------------------------------
// Normal Case: Save 후 FindById로 동일 데이터를 반환해야 한다
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, FindById_AfterSave_ShouldReturn) {
    ProductionJob job = MakeJob("PJ-002", "ORD-20260115-0002", "S-002",
                                9, 12, 360.0, 360.0);
    sut->Save(job);

    std::optional<ProductionJob> found = sut->FindById("PJ-002");

    ASSERT_TRUE(found.has_value()) << "저장된 Job을 FindById로 찾을 수 있어야 한다";
    EXPECT_EQ(found->productionId,             "PJ-002");
    EXPECT_EQ(found->orderId,                  "ORD-20260115-0002");
    EXPECT_EQ(found->sampleId,                 "S-002");
    EXPECT_EQ(found->requiredQuantity,         9);
    EXPECT_EQ(found->actualProductionQuantity, 12);
    EXPECT_DOUBLE_EQ(found->totalProductionTime, 360.0);
}

// -----------------------------------------------------------------------
// Normal Case: FindByOrderId는 해당 orderId를 가진 Job만 반환해야 한다
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, FindByOrderId_ShouldReturnMatchingJobs) {
    sut->Save(MakeJob("PJ-010", "ORD-A", "S-001"));
    sut->Save(MakeJob("PJ-011", "ORD-A", "S-002"));  // 같은 orderId
    sut->Save(MakeJob("PJ-012", "ORD-B", "S-003"));  // 다른 orderId

    std::vector<ProductionJob> results = sut->FindByOrderId("ORD-A");

    EXPECT_EQ(results.size(), 2u) << "ORD-A에 해당하는 Job이 2개여야 한다";
    for (const auto& j : results) {
        EXPECT_EQ(j.orderId, "ORD-A");
    }
}

// -----------------------------------------------------------------------
// Normal Case: FindAll은 저장된 모든 Job을 반환해야 한다
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, FindAll_ShouldReturnAllSaved) {
    sut->Save(MakeJob("PJ-020", "ORD-20260115-0020", "S-001"));
    sut->Save(MakeJob("PJ-021", "ORD-20260115-0021", "S-002"));
    sut->Save(MakeJob("PJ-022", "ORD-20260115-0022", "S-003"));

    std::vector<ProductionJob> all = sut->FindAll();

    EXPECT_EQ(all.size(), 3u) << "저장된 Job 3개가 모두 반환되어야 한다";
}

// -----------------------------------------------------------------------
// Normal Case: 파일 로드 — 새 인스턴스 생성 시 저장된 데이터를 복원해야 한다
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, Load_ShouldRestoreFromFile) {
    sut->Save(MakeJob("PJ-030", "ORD-LOAD-001", "S-001", 5, 7, 210.0, 210.0));
    sut->Save(MakeJob("PJ-031", "ORD-LOAD-002", "S-002", 3, 4, 120.0, 120.0));

    Reload();

    std::optional<ProductionJob> pj030 = sut->FindById("PJ-030");
    ASSERT_TRUE(pj030.has_value()) << "재로드 후 PJ-030을 찾을 수 있어야 한다";
    EXPECT_EQ(pj030->orderId, "ORD-LOAD-001");
    EXPECT_EQ(pj030->requiredQuantity, 5);
    EXPECT_EQ(pj030->actualProductionQuantity, 7);

    std::optional<ProductionJob> pj031 = sut->FindById("PJ-031");
    ASSERT_TRUE(pj031.has_value()) << "재로드 후 PJ-031을 찾을 수 있어야 한다";
    EXPECT_EQ(pj031->orderId, "ORD-LOAD-002");
}

// -----------------------------------------------------------------------
// Abnormal Case: 존재하지 않는 ID 조회 → nullopt 반환
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, FindById_NotFound_ShouldReturnNullopt) {
    std::optional<ProductionJob> result = sut->FindById("PJ-INVALID");

    EXPECT_FALSE(result.has_value()) << "미등록 ID 조회 시 nullopt 여야 한다";
}

// -----------------------------------------------------------------------
// Abnormal Case: FindByOrderId — 해당하는 Job 없으면 빈 벡터 반환
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, FindByOrderId_NoMatch_ShouldReturnEmpty) {
    sut->Save(MakeJob("PJ-040", "ORD-X", "S-001"));

    std::vector<ProductionJob> results = sut->FindByOrderId("ORD-NOT-EXIST");

    EXPECT_TRUE(results.empty()) << "매칭되는 Job이 없으면 빈 벡터를 반환해야 한다";
}

// -----------------------------------------------------------------------
// Abnormal Case: FindAll — 빈 저장소이면 빈 벡터 반환
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, FindAll_EmptyRepository_ShouldReturnEmpty) {
    std::vector<ProductionJob> all = sut->FindAll();

    EXPECT_TRUE(all.empty()) << "저장된 데이터 없으면 FindAll은 빈 벡터여야 한다";
}

// -----------------------------------------------------------------------
// Abnormal Case: 동일 productionId Save 두 번 → 마지막 값으로 덮어써야 한다
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, Save_DuplicateId_ShouldOverwrite) {
    sut->Save(MakeJob("PJ-050", "ORD-DUP", "S-001", 5, 7, 210.0, 210.0));
    sut->Save(MakeJob("PJ-050", "ORD-DUP", "S-001", 9, 11, 330.0, 330.0));

    std::optional<ProductionJob> found = sut->FindById("PJ-050");
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found->requiredQuantity,         9) << "두 번째 Save 값으로 덮어써야 한다";
    EXPECT_EQ(found->actualProductionQuantity, 11);
    EXPECT_DOUBLE_EQ(found->totalProductionTime, 330.0);
}

// -----------------------------------------------------------------------
// Boundary Case: startTime과 expectedFinishTime이 같은 Job (즉시 완료)
// -----------------------------------------------------------------------
TEST_F(ProductionJobJsonRepositoryFixture, Save_ZeroProductionTime_ShouldPersistCorrectly) {
    ProductionJob job = MakeJob("PJ-060", "ORD-ZERO", "S-001", 1, 1, 0.0, 0.0);
    job.expectedFinishTime = job.startTime;  // 즉시 완료
    sut->Save(job);

    std::optional<ProductionJob> found = sut->FindById("PJ-060");
    ASSERT_TRUE(found.has_value());
    EXPECT_DOUBLE_EQ(found->totalProductionTime, 0.0);
    EXPECT_EQ(found->startTime, found->expectedFinishTime)
        << "startTime과 expectedFinishTime이 동일해야 한다";
}
