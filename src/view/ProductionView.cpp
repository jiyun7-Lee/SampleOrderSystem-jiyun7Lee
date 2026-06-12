#include "ProductionView.h"
#include <iostream>
#include <iomanip>
#include <ctime>

// static: time_point -> "YYYY-MM-DD HH:MM:SS" 문자열 변환
std::string ProductionView::FormatTimePoint(
    const std::chrono::system_clock::time_point& tp)
{
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tmBuf{};
#ifdef _WIN32
    localtime_s(&tmBuf, &t);
#else
    localtime_r(&t, &tmBuf);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmBuf);
    return std::string(buf);
}

// 현재 생산 중인 작업 한 줄 출력
void ProductionView::PrintCurrentJob(const ProductionJob& job) const {
    std::cout << "[현재 생산 중]\n"
              << "주문번호: " << job.orderId
              << " | 시료ID: " << job.sampleId
              << " | 수량: " << job.requiredQuantity
              << " | 완료 예정: " << FormatTimePoint(job.expectedFinishTime)
              << "\n";
}

// 대기 큐 테이블 출력 (FIFO 순서)
void ProductionView::PrintWaitingJobs(const std::vector<ProductionJob>& jobs) const {
    std::cout << "\n[대기 중인 작업 목록 (FIFO 순서)]\n"
              << "No | 주문번호             | 시료ID  | 수량\n"
              << "-------------------------------------------\n";
    int no = 1;
    for (const auto& job : jobs) {
        std::cout << std::setw(2) << no++
                  << " | " << job.orderId
                  << " | " << job.sampleId
                  << " | " << job.requiredQuantity
                  << "\n";
    }
}

void ProductionView::ShowProductionLine(
    const std::optional<ProductionJob>& currentJob,
    const std::vector<ProductionJob>& waitingJobs) const
{
    std::cout << "\n[생산 라인 조회]\n";

    if (!currentJob.has_value() && waitingJobs.empty()) {
        std::cout << "현재 생산 중인 작업이 없습니다.\n";
        return;
    }

    if (currentJob.has_value()) {
        PrintCurrentJob(currentJob.value());
    }

    if (!waitingJobs.empty()) {
        PrintWaitingJobs(waitingJobs);
    }
}

void ProductionView::ShowMessage(const std::string& msg) const {
    std::cout << msg << "\n";
}
