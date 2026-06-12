#include "ProductionView.h"
#include "ViewUtil.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

// 컬럼 표시 너비 (display columns)
static constexpr int kColNo        =  3;
static constexpr int kColOrderId   = 20;
static constexpr int kColSampleId  =  8;
static constexpr int kColQty       =  6;
static constexpr int kColFinishAt  = 20;
static constexpr int kSeparatorLen =
    kColNo + kColOrderId + kColSampleId + kColQty + kColFinishAt + 12;
//  12 = " | " x 4

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

void ProductionView::PrintCurrentJob(const ProductionJob& job, double progressPercent) const {
    using namespace ViewUtil;

    // 진행률 바 (20칸)
    constexpr int kBarWidth = 20;
    int filled = static_cast<int>(progressPercent / 100.0 * kBarWidth);
    std::string bar(filled, '#');
    bar += std::string(kBarWidth - filled, '-');

    std::ostringstream pctStr;
    pctStr << std::fixed << std::setprecision(1) << progressPercent << "%";

    std::cout << "\n[현재 생산 중]\n"
              << "  " << PadRight("주문번호",   14) << ": " << job.orderId          << "\n"
              << "  " << PadRight("시료ID",     14) << ": " << job.sampleId         << "\n"
              << "  " << PadRight("필요 수량",  14) << ": " << job.requiredQuantity  << " ea\n"
              << "  " << PadRight("진행률",     14) << ": [" << bar << "] " << pctStr.str() << "\n"
              << "  " << PadRight("완료 예정",  14) << ": " << FormatTimePoint(job.expectedFinishTime) << "\n";
}

void ProductionView::PrintWaitingJobs(const std::vector<ProductionJob>& jobs) const {
    using namespace ViewUtil;
    std::cout << "\n[대기 중인 작업 목록 (FIFO 순서)]\n"
              << PadLeft ("No",       kColNo)       << " | "
              << PadRight("주문번호", kColOrderId)  << " | "
              << PadRight("시료ID",   kColSampleId) << " | "
              << PadLeft ("수량",     kColQty)      << " | "
              << PadRight("완료 예정", kColFinishAt)
              << "\n" << std::string(kSeparatorLen, '-') << "\n";

    int no = 1;
    for (const auto& job : jobs) {
        std::ostringstream noStr, qty;
        noStr << no++;
        qty   << job.requiredQuantity;
        std::cout << PadLeft (noStr.str(),                      kColNo)       << " | "
                  << PadRight(job.orderId,                      kColOrderId)  << " | "
                  << PadRight(job.sampleId,                     kColSampleId) << " | "
                  << PadLeft (qty.str(),                        kColQty)      << " | "
                  << PadRight(FormatTimePoint(job.expectedFinishTime), kColFinishAt)
                  << "\n";
    }
}

void ProductionView::ShowProductionLine(
    const std::optional<ProductionProgress>& currentProgress,
    const std::vector<ProductionJob>& waitingJobs) const
{
    std::cout << "\n[생산 라인 조회]\n";

    if (!currentProgress.has_value() && waitingJobs.empty()) {
        std::cout << "현재 생산 중인 작업이 없습니다.\n";
        return;
    }

    if (currentProgress.has_value()) {
        PrintCurrentJob(currentProgress->job, currentProgress->progressPercent);
    }

    if (!waitingJobs.empty()) {
        PrintWaitingJobs(waitingJobs);
    }
}

void ProductionView::ShowMessage(const std::string& msg) const {
    std::cout << msg << "\n";
}
