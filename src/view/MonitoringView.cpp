#include "MonitoringView.h"
#include <iostream>
#include <limits>

namespace {
    constexpr const char* kInventorySeparator =
        "-------------------------------------------\n";
}

void MonitoringView::ShowMenu() const {
    std::cout << "\n[모니터링]\n"
              << "  [1] 주문량 확인\n"
              << "  [2] 재고량 확인\n"
              << "  [0] 뒤로\n";
}

void MonitoringView::ShowOrderSummary(const OrderStatusSummary& summary) const {
    std::cout << "\n[주문량 현황]\n"
              << "  접수(RESERVED) : " << summary.reservedCount  << "건\n"
              << "  생산 중(PRODUCING): " << summary.producingCount << "건\n"
              << "  출고 대기(CONFIRMED): " << summary.confirmedCount << "건\n"
              << "  출고 완료(RELEASE): " << summary.releaseCount   << "건\n";
}

void MonitoringView::ShowInventoryStatus(const std::vector<InventoryStatus>& statuses) const {
    if (statuses.empty()) {
        std::cout << "등록된 시료가 없습니다.\n";
        return;
    }
    std::cout << "\n[재고 현황]\n"
              << "시료ID  | 시료명         | 현재 재고 | 상태\n"
              << kInventorySeparator;
    for (const auto& s : statuses) {
        std::cout << s.sampleId
                  << " | " << s.sampleName
                  << " | " << s.currentStock
                  << " | " << StockStatusToString(s.status)
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
