#include "ReleaseView.h"
#include "ViewUtil.h"
#include <iostream>
#include <sstream>
#include <limits>

// 컬럼 표시 너비 (display columns)
static constexpr int kColNo        =  4;
static constexpr int kColOrderId   = 20;
static constexpr int kColSampleId  =  8;
static constexpr int kColCustomer  = 16;
static constexpr int kColQty       =  6;
static constexpr int kSeparatorLen =
    kColNo + kColOrderId + kColSampleId + kColCustomer + kColQty + 12;
//  12 = " | " x 4

void ReleaseView::ShowConfirmedOrders(const std::vector<Order>& orders) const {
    using namespace ViewUtil;
    std::cout << "\n[출고 처리] CONFIRMED 주문 목록\n"
              << PadLeft ("번호",   kColNo)       << " | "
              << PadRight("주문번호", kColOrderId) << " | "
              << PadRight("시료ID",  kColSampleId) << " | "
              << PadRight("고객명",  kColCustomer) << " | "
              << PadLeft ("수량",    kColQty)
              << "\n" << std::string(kSeparatorLen, '-') << "\n";

    int idx = 1;
    for (const auto& o : orders) {
        std::ostringstream no, qty;
        no  << idx++;
        qty << o.quantity;
        std::cout << PadLeft (no.str(),        kColNo)       << " | "
                  << PadRight(o.orderId,       kColOrderId)  << " | "
                  << PadRight(o.sampleId,      kColSampleId) << " | "
                  << PadRight(o.customerName,  kColCustomer) << " | "
                  << PadLeft (qty.str(),        kColQty)
                  << "\n";
    }
}

void ReleaseView::ShowReleaseResult(const Order& order) const {
    using namespace ViewUtil;
    std::cout << "\n[출고 완료]\n"
              << "  " << PadRight("주문번호", 10) << ": " << order.orderId              << "\n"
              << "  " << PadRight("수량",     10) << ": " << order.quantity             << "\n"
              << "  " << PadRight("상태",     10) << ": " << StatusToString(order.status) << "\n";
}

void ReleaseView::ShowMessage(const std::string& msg) const {
    std::cout << msg << std::endl;
}

int ReleaseView::InputInt(const std::string& prompt) const {
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

std::string ReleaseView::StatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::RESERVED:  return "접수";
        case OrderStatus::REJECTED:  return "거절";
        case OrderStatus::PRODUCING: return "생산 중";
        case OrderStatus::CONFIRMED: return "출고 대기";
        case OrderStatus::RELEASE:   return "출고 완료";
        default:                     return "알 수 없음";
    }
}
