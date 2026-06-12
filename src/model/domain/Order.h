#pragma once

#include <string>
#include <chrono>
#include "OrderStatus.h"

struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int quantity;
    OrderStatus status;
    std::chrono::system_clock::time_point createdAt;
};
