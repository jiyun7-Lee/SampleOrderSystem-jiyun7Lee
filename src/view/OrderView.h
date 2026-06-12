#pragma once
#include <vector>
#include <string>
#include "../model/domain/Order.h"

class OrderView {
public:
    void ShowOrderMenu() const;
    void ShowApprovalMenu() const;
    void ShowOrderList(const std::vector<Order>& orders) const;
    void ShowMessage(const std::string& msg) const;
    std::string InputString(const std::string& prompt) const;
    int InputInt(const std::string& prompt) const;

private:
    static std::string StatusToString(OrderStatus status);
};
