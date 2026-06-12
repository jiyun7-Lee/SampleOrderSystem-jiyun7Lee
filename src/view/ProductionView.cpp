#include "ProductionView.h"
#include <iostream>
#include <iomanip>
#include <ctime>

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
        const auto& job = currentJob.value();
        std::time_t finishT = std::chrono::system_clock::to_time_t(job.expectedFinishTime);
        std::tm tmBuf{};
#ifdef _WIN32
        localtime_s(&tmBuf, &finishT);
#else
        localtime_r(&finishT, &tmBuf);
#endif
        char timeBuf[32];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &tmBuf);

        std::cout << "[현재 생산 중]\n"
                  << "주문번호: " << job.orderId
                  << " | 시료ID: " << job.sampleId
                  << " | 수량: " << job.requiredQuantity
                  << " | 완료 예정: " << timeBuf
                  << "\n";
    }

    if (!waitingJobs.empty()) {
        std::cout << "\n[대기 중인 작업 목록 (FIFO 순서)]\n"
                  << "No | 주문번호             | 시료ID  | 수량\n"
                  << "-------------------------------------------\n";
        int no = 1;
        for (const auto& job : waitingJobs) {
            std::cout << std::setw(2) << no++
                      << " | " << job.orderId
                      << " | " << job.sampleId
                      << " | " << job.requiredQuantity
                      << "\n";
        }
    }
}

void ProductionView::ShowMessage(const std::string& msg) const {
    std::cout << msg << std::endl;
}
