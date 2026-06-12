#pragma once
#include <vector>
#include <string>
#include "../model/domain/Order.h"

class ReleaseView {
public:
    void ShowConfirmedOrders(const std::vector<Order>& orders) const;
    void ShowReleaseResult(const Order& order) const;
    void ShowMessage(const std::string& msg) const;
    int InputInt(const std::string& prompt) const;

private:
    static std::string StatusToString(OrderStatus status);
};
