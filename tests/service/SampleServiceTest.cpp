#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../src/model/repository/ISampleRepository.h"
#include "../../src/model/domain/Sample.h"
#include "../../src/service/SampleService.h"
#include <stdexcept>
#include <memory>
#include <optional>
#include <vector>
#include <string>

using ::testing::Return;
using ::testing::_;

// -----------------------------------------------------------------------
// Mock
// -----------------------------------------------------------------------
class MockSampleRepository : public ISampleRepository {
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
class SampleServiceFixture : public ::testing::Test {
protected:
    std::shared_ptr<MockSampleRepository> mockRepo;
    std::unique_ptr<SampleService> service;

    void SetUp() override {
        mockRepo = std::make_shared<MockSampleRepository>();
        service = std::make_unique<SampleService>(mockRepo);
    }

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

// -----------------------------------------------------------------------
// Normal Cases
// -----------------------------------------------------------------------

// FR-01: 시료 등록 — 유효한 시료는 Save 1회 호출
TEST_F(SampleServiceFixture, RegisterSample_ValidSample_ShouldCallSave) {
    Sample sample = MakeSample("S-001", "AlphaSample");

    EXPECT_CALL(*mockRepo, ExistsById("S-001"))
        .Times(1)
        .WillOnce(Return(false));
    EXPECT_CALL(*mockRepo, Save(::testing::_))
        .Times(1);

    service->RegisterSample(sample);
}

// FR-02: 전체 시료 목록 조회 — FindAll 결과를 그대로 반환
TEST_F(SampleServiceFixture, GetAllSamples_ShouldReturnAllFromRepository) {
    std::vector<Sample> samples = {
        MakeSample("S-001", "AlphaSample"),
        MakeSample("S-002", "BetaSample")
    };

    EXPECT_CALL(*mockRepo, FindAll())
        .Times(1)
        .WillOnce(Return(samples));

    auto result = service->GetAllSamples();

    EXPECT_EQ(result.size(), 2u);
}

// FR-02: ID로 시료 단건 조회 — 존재하는 ID는 값을 반환
TEST_F(SampleServiceFixture, FindSampleById_ExistingId_ShouldReturnSample) {
    Sample sample = MakeSample("S-001", "AlphaSample");

    EXPECT_CALL(*mockRepo, FindById("S-001"))
        .Times(1)
        .WillOnce(Return(std::optional<Sample>{sample}));

    auto result = service->FindSampleById("S-001");

    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->sampleId, "S-001");
}

// FR-03: 이름으로 시료 검색 — 매칭 결과 반환
TEST_F(SampleServiceFixture, SearchSamplesByName_ShouldReturnMatchingResults) {
    std::vector<Sample> matched = { MakeSample("S-001", "AlphaSample") };

    EXPECT_CALL(*mockRepo, FindByName("Alpha"))
        .Times(1)
        .WillOnce(Return(matched));

    auto result = service->SearchByName("Alpha");

    EXPECT_EQ(result.size(), 1u);
}

// -----------------------------------------------------------------------
// Abnormal Cases
// -----------------------------------------------------------------------

// FR-01: 중복 ID로 시료 등록 시 runtime_error throw
TEST_F(SampleServiceFixture, RegisterSample_DuplicateId_ShouldThrow) {
    Sample sample = MakeSample("S-001", "AlphaSample");

    EXPECT_CALL(*mockRepo, ExistsById("S-001"))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_CALL(*mockRepo, Save(::testing::_))
        .Times(0);

    EXPECT_THROW(service->RegisterSample(sample), std::runtime_error);
}

// FR-01: 빈 ID로 시료 등록 시 invalid_argument throw
//        ExistsById는 호출되지 않아야 함
TEST_F(SampleServiceFixture, RegisterSample_EmptyId_ShouldThrow) {
    Sample sample = MakeSample("", "AlphaSample");

    EXPECT_CALL(*mockRepo, ExistsById(::testing::_))
        .Times(0);
    EXPECT_CALL(*mockRepo, Save(::testing::_))
        .Times(0);

    EXPECT_THROW(service->RegisterSample(sample), std::invalid_argument);
}

// FR-01: 음수 수율(yieldRate < 0)로 시료 등록 시 invalid_argument throw
TEST_F(SampleServiceFixture, RegisterSample_NegativeYield_ShouldThrow) {
    Sample sample = MakeSample("S-003", "GammaSample", 10.0, -0.1);

    EXPECT_CALL(*mockRepo, ExistsById(::testing::_))
        .Times(0);
    EXPECT_CALL(*mockRepo, Save(::testing::_))
        .Times(0);

    EXPECT_THROW(service->RegisterSample(sample), std::invalid_argument);
}

// FR-01: 수율이 1.0 초과(yieldRate > 1.0)로 시료 등록 시 invalid_argument throw
TEST_F(SampleServiceFixture, RegisterSample_YieldOverOne_ShouldThrow) {
    Sample sample = MakeSample("S-004", "DeltaSample", 10.0, 1.1);

    EXPECT_CALL(*mockRepo, ExistsById(::testing::_))
        .Times(0);
    EXPECT_CALL(*mockRepo, Save(::testing::_))
        .Times(0);

    EXPECT_THROW(service->RegisterSample(sample), std::invalid_argument);
}

// FR-01: 음수 평균 생산시간(averageProductionTime < 0)으로 시료 등록 시 invalid_argument throw
TEST_F(SampleServiceFixture, RegisterSample_NegativeAvgProductionTime_ShouldThrow) {
    Sample sample = MakeSample("S-005", "EpsilonSample", -1.0, 0.9);

    EXPECT_CALL(*mockRepo, ExistsById(::testing::_))
        .Times(0);
    EXPECT_CALL(*mockRepo, Save(::testing::_))
        .Times(0);

    EXPECT_THROW(service->RegisterSample(sample), std::invalid_argument);
}

// FR-02: 존재하지 않는 ID로 조회 시 nullopt 반환
TEST_F(SampleServiceFixture, FindSampleById_NonExistingId_ShouldReturnNullopt) {
    EXPECT_CALL(*mockRepo, FindById("X-999"))
        .Times(1)
        .WillOnce(Return(std::nullopt));

    auto result = service->FindSampleById("X-999");

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result, std::nullopt);
}
