#include "OrderView.h"
#include <iostream>
#include <limits>

void OrderView::ShowOrderMenu() const {
    std::cout << "\n[시료 주문]\n"
              << "시료 ID, 고객명, 수량을 입력하세요.\n";
}

void OrderView::ShowApprovalMenu() const {
    std::cout << "\n[주문 승인/거절]\n"
              << "RESERVED 상태 주문 목록입니다.\n";
}

void OrderView::ShowOrderList(const std::vector<Order>& orders) const {
    if (orders.empty()) {
        std::cout << "조회할 주문이 없습니다.\n";
        return;
    }
    std::cout << "주문번호             | 시료ID  | 고객명         | 수량 | 상태\n"
              << "--------------------------------------------------------------\n";
    for (const auto& o : orders) {
        std::cout << o.orderId
                  << " | " << o.sampleId
                  << " | " << o.customerName
                  << " | " << o.quantity
                  << " | " << StatusToString(o.status)
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
