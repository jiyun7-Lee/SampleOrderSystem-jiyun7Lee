#include "gtest/gtest.h"
#include "../../src/util/ITimeProvider.h"
#include "../../src/util/SystemTimeProvider.h"

#include <chrono>

// ============================================================
// ITimeProvider / SystemTimeProvider 테스트
// ============================================================
class SystemTimeProviderTest : public ::testing::Test {
protected:
    SystemTimeProvider provider;
};

TEST_F(SystemTimeProviderTest, Now_ShouldReturnCurrentTime) {
    auto before = std::chrono::system_clock::now();
    auto providerTime = provider.Now();
    auto after = std::chrono::system_clock::now();

    // provider.Now()는 before ~ after 사이여야 함
    auto diffBefore = std::chrono::duration_cast<std::chrono::seconds>(
        providerTime - before).count();
    auto diffAfter = std::chrono::duration_cast<std::chrono::seconds>(
        after - providerTime).count();

    // system_clock::now()와 1초 이내 차이임을 확인
    EXPECT_LE(diffBefore, 1);
    EXPECT_LE(diffAfter, 1);
}
