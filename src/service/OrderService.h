#pragma once

#include <string>
#include <vector>
#include "../model/repository/IOrderRepository.h"
#include "../model/repository/ISampleRepository.h"
#include "../model/repository/IProductionRepository.h"
#include "../model/domain/Order.h"
#include "../util/ITimeProvider.h"
#include "InventoryService.h"

class OrderService {
public:
    OrderService(
        IOrderRepository& orderRepo,
        ISampleRepository& sampleRepo,
        InventoryService& inventoryService,
        IProductionRepository& productionRepo,
        ITimeProvider& timeProvider);

    void PlaceOrder(const std::string& sampleId, const std::string& customerName, int quantity);
    void ApproveOrder(const std::string& orderId);
    void RejectOrder(const std::string& orderId);
    std::vector<Order> GetReservedOrders();

private:
    IOrderRepository&    orderRepo_;
    ISampleRepository&   sampleRepo_;
    InventoryService&    inventoryService_;
    IProductionRepository& productionRepo_;
    ITimeProvider&       timeProvider_;
    int                  orderCounter_;
};
