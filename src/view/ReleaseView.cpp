#include "ReleaseView.h"
#include <iostream>
#include <limits>

void ReleaseView::ShowConfirmedOrders(const std::vector<Order>& orders) const {
    std::cout << "\n[출고 처리] CONFIRMED 주문 목록\n"
              << "번호 | 주문번호             | 시료ID  | 고객명         | 수량\n"
              << "---------------------------------------------------------------\n";
    int idx = 1;
    for (const auto& o : orders) {
        std::cout << idx++
                  << "    | " << o.orderId
                  << " | " << o.sampleId
                  << " | " << o.customerName
                  << " | " << o.quantity
                  << "\n";
    }
}

void ReleaseView::ShowReleaseResult(const Order& order) const {
    std::cout << "\n[출고 완료]\n"
              << "  주문번호: " << order.orderId << "\n"
              << "  수량    : " << order.quantity << "\n"
              << "  상태    : " << StatusToString(order.status) << "\n";
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
