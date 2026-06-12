#pragma once
#include <vector>
#include <string>
#include "../model/repository/IOrderRepository.h"
#include "InventoryService.h"

class ReleaseService {
public:
    ReleaseService(IOrderRepository& orderRepo, InventoryService& inventoryService);
    std::vector<Order> GetConfirmedOrders() const;
    void ReleaseOrder(const std::string& orderId);
private:
    IOrderRepository& orderRepo_;
    InventoryService& inventoryService_;
};
