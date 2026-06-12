#include "MonitoringView.h"
#include "ViewUtil.h"
#include <iostream>
#include <sstream>
#include <limits>

// 컬럼 표시 너비 (display columns)
static constexpr int kColSampleId   =  8;
static constexpr int kColName       = 20;
static constexpr int kColStock      = 10;
static constexpr int kColStockStat  =  8;
static constexpr int kSeparatorLen  =
    kColSampleId + kColName + kColStock + kColStockStat + 9;
//  9 = " | " x 3

// 주문량 현황 라벨 너비 (표시 너비 기준)
static constexpr int kLabelWidth = 24;

void MonitoringView::ShowMenu() const {
    std::cout << "\n[모니터링]\n"
              << "  [1] 주문량 확인\n"
              << "  [2] 재고량 확인\n"
              << "  [0] 뒤로\n"
              << "선택: ";
}

void MonitoringView::ShowOrderSummary(const OrderStatusSummary& summary) const {
    using namespace ViewUtil;
    std::cout << "\n[주문량 현황]\n"
              << "  " << PadRight("접수(RESERVED)",      kLabelWidth) << ": " << summary.reservedCount  << "건\n"
              << "  " << PadRight("생산 중(PRODUCING)",  kLabelWidth) << ": " << summary.producingCount << "건\n"
              << "  " << PadRight("출고 대기(CONFIRMED)", kLabelWidth) << ": " << summary.confirmedCount << "건\n"
              << "  " << PadRight("출고 완료(RELEASE)",  kLabelWidth) << ": " << summary.releaseCount   << "건\n";
}

void MonitoringView::ShowInventoryStatus(const std::vector<InventoryStatus>& statuses) const {
    using namespace ViewUtil;
    if (statuses.empty()) {
        std::cout << "등록된 시료가 없습니다.\n";
        return;
    }
    std::cout << "\n[재고 현황]\n"
              << PadRight("시료ID",   kColSampleId) << " | "
              << PadRight("시료명",   kColName)     << " | "
              << PadLeft ("현재 재고", kColStock)   << " | "
              << PadRight("상태",     kColStockStat)
              << "\n" << std::string(kSeparatorLen, '-') << "\n";

    for (const auto& s : statuses) {
        std::ostringstream stock;
        stock << s.currentStock;
        std::cout << PadRight(s.sampleId,                     kColSampleId) << " | "
                  << PadRight(s.sampleName,                   kColName)     << " | "
                  << PadLeft (stock.str(),                    kColStock)    << " | "
                  << PadRight(StockStatusToString(s.status),  kColStockStat)
                  << "\n";
    }
}

void MonitoringView::ShowMessage(const std::string& msg) const {
    std::cout << msg << std::endl;
}

int MonitoringView::InputInt(const std::string& prompt) const {
    std::cout << prompt;
    int value = -1;
    std::cin >> value;
    if (std::cin.fail()) {
        std::cin.clear();
        value = -1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

std::string MonitoringView::StockStatusToString(StockStatus status) {
    switch (status) {
        case StockStatus::SUFFICIENT: return "여유";
        case StockStatus::SHORTAGE:   return "부족";
        case StockStatus::DEPLETED:   return "고갈";
        default:                      return "알 수 없음";
    }
}
