#include "MonitoringService.h"

MonitoringService::MonitoringService(IOrderRepository& orderRepo,
                                     IInventoryRepository& inventoryRepo,
                                     ISampleRepository& sampleRepo)
    : orderRepo_(orderRepo)
    , inventoryRepo_(inventoryRepo)
    , sampleRepo_(sampleRepo)
{
}

OrderStatusSummary MonitoringService::GetOrderSummary() const {
    OrderStatusSummary summary;
    summary.reservedCount  = static_cast<int>(orderRepo_.FindByStatus(OrderStatus::RESERVED).size());
    summary.producingCount = static_cast<int>(orderRepo_.FindByStatus(OrderStatus::PRODUCING).size());
    summary.confirmedCount = static_cast<int>(orderRepo_.FindByStatus(OrderStatus::CONFIRMED).size());
    summary.releaseCount   = static_cast<int>(orderRepo_.FindByStatus(OrderStatus::RELEASE).size());
    return summary;
}

std::vector<InventoryStatus> MonitoringService::GetInventoryStatus() const {
    std::vector<InventoryStatus> result;

    for (const auto& sample : sampleRepo_.FindAll()) {
        const auto inv = inventoryRepo_.FindBySampleId(sample.sampleId);
        const int stock = inv ? inv->currentStock : 0;

        StockStatus stockStatus;
        if (stock == 0) {
            stockStatus = StockStatus::DEPLETED;
        } else if (inv->availableStock() <= 0) {
            stockStatus = StockStatus::SHORTAGE;
        } else {
            stockStatus = StockStatus::SUFFICIENT;
        }

        result.push_back({ sample.sampleId, sample.name, stock, stockStatus });
    }

    return result;
}
