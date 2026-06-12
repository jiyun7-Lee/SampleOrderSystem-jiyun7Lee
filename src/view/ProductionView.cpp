#include "ProductionView.h"
#include "ViewUtil.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

// 컬럼 표시 너비 (display columns)
static constexpr int kColNo       =  3;
static constexpr int kColOrderId  = 20;
static constexpr int kColSampleId =  8;
static constexpr int kColQty      =  6;
static constexpr int kSeparatorLen =
    kColNo + kColOrderId + kColSampleId + kColQty + 9;
//  9 = " | " x 3

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

void ProductionView::PrintCurrentJob(const ProductionJob& job) const {
    using namespace ViewUtil;
    std::cout << "\n[현재 생산 중]\n"
              << "  " << PadRight("주문번호",   14) << ": " << job.orderId          << "\n"
              << "  " << PadRight("시료ID",     14) << ": " << job.sampleId         << "\n"
              << "  " << PadRight("필요 수량",  14) << ": " << job.requiredQuantity  << "\n"
              << "  " << PadRight("완료 예정",  14) << ": " << FormatTimePoint(job.expectedFinishTime) << "\n";
}

void ProductionView::PrintWaitingJobs(const std::vector<ProductionJob>& jobs) const {
    using namespace ViewUtil;
    std::cout << "\n[대기 중인 작업 목록 (FIFO 순서)]\n"
              << PadLeft ("No",     kColNo)       << " | "
              << PadRight("주문번호", kColOrderId) << " | "
              << PadRight("시료ID",  kColSampleId) << " | "
              << PadLeft ("수량",    kColQty)
              << "\n" << std::string(kSeparatorLen, '-') << "\n";

    int no = 1;
    for (const auto& job : jobs) {
        std::ostringstream noStr, qty;
        noStr << no++;
        qty   << job.requiredQuantity;
        std::cout << PadLeft (noStr.str(),   kColNo)       << " | "
                  << PadRight(job.orderId,   kColOrderId)  << " | "
                  << PadRight(job.sampleId,  kColSampleId) << " | "
                  << PadLeft (qty.str(),     kColQty)
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
