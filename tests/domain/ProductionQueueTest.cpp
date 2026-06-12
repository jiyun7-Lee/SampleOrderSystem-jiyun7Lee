#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../../src/model/domain/ProductionJob.h"
#include "../../src/model/domain/ProductionQueue.h"

#include <chrono>
#include <string>
#include <optional>

// ------------------------------------------------------------
// 헬퍼 팩토리 함수
// ------------------------------------------------------------
static ProductionJob MakeJob(const std::string& orderId) {
    ProductionJob job;
    job.productionId  = "PROD-" + orderId;
    job.orderId       = orderId;
    job.sampleId      = "S-001";
    job.requiredQuantity          = 10;
    job.actualProductionQuantity  = 12;
    job.totalProductionTime       = 120.0;
    job.startTime                 = std::chrono::system_clock::now();
    job.expectedFinishTime        = job.startTime + std::chrono::seconds(120);
    return job;
}

// ------------------------------------------------------------
// Fixture
// ------------------------------------------------------------
class ProductionQueueFixture : public ::testing::Test {
protected:
    ProductionQueue queue_;

    void SetUp() override {
        // 기본적으로 빈 큐 상태에서 시작
    }
};

// ============================================================
// Normal Cases
// ============================================================

// 1. Enqueue 후 IsEmpty()가 false
TEST_F(ProductionQueueFixture, Enqueue_ShouldAddToQueue) {
    EXPECT_TRUE(queue_.IsEmpty());

    queue_.Enqueue(MakeJob("ORD-001"));

    EXPECT_FALSE(queue_.IsEmpty());
}

// 2. Enqueue 후 Peek()이 첫 번째 job 반환
TEST_F(ProductionQueueFixture, Peek_ReturnsFirstJob) {
    ProductionJob job = MakeJob("ORD-001");
    queue_.Enqueue(job);

    auto result = queue_.Peek();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-001");
    EXPECT_EQ(result->productionId, "PROD-ORD-001");
}

// 3. Enqueue 2개 후 Dequeue 1번 → Peek()이 두 번째 job 반환
TEST_F(ProductionQueueFixture, Dequeue_RemovesFirstJob) {
    queue_.Enqueue(MakeJob("ORD-001"));
    queue_.Enqueue(MakeJob("ORD-002"));

    queue_.Dequeue();

    auto result = queue_.Peek();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->orderId, "ORD-002");
}

// 4. FIFO 순서 보장 — 3개 Enqueue → Dequeue 순서가 FIFO와 일치
TEST_F(ProductionQueueFixture, FIFO_Order_ShouldBePreserved) {
    queue_.Enqueue(MakeJob("ORD-001"));
    queue_.Enqueue(MakeJob("ORD-002"));
    queue_.Enqueue(MakeJob("ORD-003"));

    auto first = queue_.Peek();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(first->orderId, "ORD-001");

    queue_.Dequeue();
    auto second = queue_.Peek();
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(second->orderId, "ORD-002");

    queue_.Dequeue();
    auto third = queue_.Peek();
    ASSERT_TRUE(third.has_value());
    EXPECT_EQ(third->orderId, "ORD-003");
}

// 5. 아무것도 없을 때 IsEmpty() == true
TEST_F(ProductionQueueFixture, IsEmpty_WhenNoJobs_ShouldBeTrue) {
    EXPECT_TRUE(queue_.IsEmpty());
}

// ============================================================
// Abnormal Cases
// ============================================================

// 6. 빈 큐에서 Peek() == std::nullopt
TEST_F(ProductionQueueFixture, Peek_WhenEmpty_ShouldReturnNullopt) {
    EXPECT_TRUE(queue_.IsEmpty());

    auto result = queue_.Peek();

    EXPECT_FALSE(result.has_value());
}

// 7. 빈 큐에서 Dequeue()는 예외 없이 no-op
TEST_F(ProductionQueueFixture, Dequeue_WhenEmpty_ShouldNotThrow) {
    EXPECT_TRUE(queue_.IsEmpty());

    EXPECT_NO_THROW(queue_.Dequeue());

    // 상태 불변: 여전히 빈 큐
    EXPECT_TRUE(queue_.IsEmpty());
}

// ============================================================
// GetWaitingJobs 테스트
// ============================================================

// 8. GetWaitingJobs — 큐가 비어 있을 때 빈 벡터 반환
TEST_F(ProductionQueueFixture, GetWaitingJobs_WhenEmpty_ShouldReturnEmptyVector) {
    auto jobs = queue_.GetWaitingJobs();

    EXPECT_TRUE(jobs.empty());
}

// 9. GetWaitingJobs — Enqueue 3개 후 3개 반환, FIFO 순서 유지
TEST_F(ProductionQueueFixture, GetWaitingJobs_ShouldReturnAllJobsInFIFOOrder) {
    queue_.Enqueue(MakeJob("ORD-001"));
    queue_.Enqueue(MakeJob("ORD-002"));
    queue_.Enqueue(MakeJob("ORD-003"));

    auto jobs = queue_.GetWaitingJobs();

    ASSERT_EQ(jobs.size(), static_cast<size_t>(3));
    EXPECT_EQ(jobs[0].orderId, "ORD-001");
    EXPECT_EQ(jobs[1].orderId, "ORD-002");
    EXPECT_EQ(jobs[2].orderId, "ORD-003");
}

// 10. GetWaitingJobs — Dequeue 후 남은 항목만 반환
TEST_F(ProductionQueueFixture, GetWaitingJobs_AfterDequeue_ShouldReflectRemainingJobs) {
    queue_.Enqueue(MakeJob("ORD-001"));
    queue_.Enqueue(MakeJob("ORD-002"));
    queue_.Enqueue(MakeJob("ORD-003"));

    queue_.Dequeue();

    auto jobs = queue_.GetWaitingJobs();

    ASSERT_EQ(jobs.size(), static_cast<size_t>(2));
    EXPECT_EQ(jobs[0].orderId, "ORD-002");
    EXPECT_EQ(jobs[1].orderId, "ORD-003");
}

// 11. Dequeue를 모두 완료한 후 IsEmpty() == true
TEST_F(ProductionQueueFixture, Dequeue_AllJobs_ShouldBecomeEmpty) {
    queue_.Enqueue(MakeJob("ORD-001"));
    queue_.Enqueue(MakeJob("ORD-002"));

    queue_.Dequeue();
    queue_.Dequeue();

    EXPECT_TRUE(queue_.IsEmpty());
    EXPECT_FALSE(queue_.Peek().has_value());
}
