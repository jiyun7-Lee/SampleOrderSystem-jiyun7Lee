#include "ReleaseService.h"
#include <stdexcept>

ReleaseService::ReleaseService(IOrderRepository& orderRepo, InventoryService& inventoryService)
    : orderRepo_(orderRepo), inventoryService_(inventoryService) {}

std::vector<Order> ReleaseService::GetConfirmedOrders() const {
    return orderRepo_.FindByStatus(OrderStatus::CONFIRMED);
}

void ReleaseService::ReleaseOrder(const std::string& orderId) {
    if (orderId.empty()) {
        throw std::invalid_argument("orderId must not be empty");
    }

    const auto optOrder = orderRepo_.FindById(orderId);
    if (!optOrder) {
        throw std::runtime_error("Order not found: " + orderId);
    }

    if (optOrder->status != OrderStatus::CONFIRMED) {
        throw std::logic_error("Order is not in CONFIRMED status: " + orderId);
    }

    inventoryService_.DeductStock(optOrder->sampleId, optOrder->quantity);
    inventoryService_.DeallocateStock(optOrder->sampleId, optOrder->quantity);

    Order updated = *optOrder;
    updated.status = OrderStatus::RELEASE;
    orderRepo_.Save(updated);
}
