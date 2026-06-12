#include "OrderView.h"
#include "ViewUtil.h"
#include <iostream>
#include <sstream>
#include <limits>

// 컬럼 표시 너비 (display columns)
static constexpr int kColNo          =  4;
static constexpr int kColOrderId     = 20;
static constexpr int kColSampleId    =  8;
static constexpr int kColCustomer    = 16;
static constexpr int kColQty         =  6;
static constexpr int kColStatus      = 10;
static constexpr int kSeparatorLen   =
    kColNo + kColOrderId + kColSampleId + kColCustomer + kColQty + kColStatus + 15;
//  15 = " | " x 5

void OrderView::ShowOrderMenu() const {
    std::cout << "\n[시료 주문]\n"
              << "시료 ID, 고객명, 수량을 입력하세요.\n";
}

void OrderView::ShowApprovalMenu() const {
    std::cout << "\n[주문 승인/거절]\n"
              << "RESERVED 상태 주문 목록입니다.\n";
}

void OrderView::ShowOrderList(const std::vector<Order>& orders) const {
    using namespace ViewUtil;
    if (orders.empty()) {
        std::cout << "조회할 주문이 없습니다.\n";
        return;
    }
    std::cout << "\n"
              << PadLeft ("번호",     kColNo)       << " | "
              << PadRight("주문번호", kColOrderId)  << " | "
              << PadRight("시료ID",   kColSampleId) << " | "
              << PadRight("고객명",   kColCustomer) << " | "
              << PadLeft ("수량",     kColQty)      << " | "
              << PadRight("상태",     kColStatus)
              << "\n" << std::string(kSeparatorLen, '-') << "\n";

    int no = 1;
    for (const auto& o : orders) {
        std::ostringstream noStr, qty;
        noStr << no++;
        qty   << o.quantity;
        std::cout << PadLeft (noStr.str(),              kColNo)       << " | "
                  << PadRight(o.orderId,                kColOrderId)  << " | "
                  << PadRight(o.sampleId,               kColSampleId) << " | "
                  << PadRight(o.customerName,           kColCustomer) << " | "
                  << PadLeft (qty.str(),                kColQty)      << " | "
                  << PadRight(StatusToString(o.status), kColStatus)
                  << "\n";
    }
}

void OrderView::ShowMessage(const std::string& msg) const {
    std::cout << msg << std::endl;
}

std::string OrderView::InputString(const std::string& prompt) const {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

int OrderView::InputInt(const std::string& prompt) const {
    std::cout << prompt;
    int value = -1;
    std::cin >> value;
    if (std::cin.fail()) {
        std::cin.clear();
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

std::string OrderView::StatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::RESERVED:  return "접수";
        case OrderStatus::REJECTED:  return "거절";
        case OrderStatus::PRODUCING: return "생산 중";
        case OrderStatus::CONFIRMED: return "출고 대기";
        case OrderStatus::RELEASE:   return "출고 완료";
        default:                     return "알 수 없음";
    }
}
