#include "ReleaseService.h"
#include <stdexcept>

ReleaseService::ReleaseService(IOrderRepository& orderRepo, InventoryService& inventoryService)
    : orderRepo_(orderRepo), inventoryService_(inventoryService) {}

std::vector<Order> ReleaseService::GetConfirmedOrders() const {
    return orderRepo_.FindByStatus(OrderStatus::CONFIRMED);
}

void ReleaseService::ReleaseOrder(const std::string& orderId) {
    auto optOrder = orderRepo_.FindById(orderId);
    if (!optOrder.has_value()) {
        throw std::runtime_error("Order not found: " + orderId);
    }

    Order order = *optOrder;
    if (order.status != OrderStatus::CONFIRMED) {
        throw std::logic_error("Order is not in CONFIRMED status: " + orderId);
    }

    inventoryService_.DeductStock(order.sampleId, order.quantity);
    inventoryService_.DeallocateStock(order.sampleId, order.quantity);

    order.status = OrderStatus::RELEASE;
    orderRepo_.Save(order);
}
