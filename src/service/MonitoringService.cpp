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
    const auto samples = sampleRepo_.FindAll();

    for (const auto& sample : samples) {
        InventoryStatus entry;
        entry.sampleId   = sample.sampleId;
        entry.sampleName = sample.name;

        const auto inv = inventoryRepo_.FindBySampleId(sample.sampleId);
        if (!inv.has_value() || inv->currentStock == 0) {
            entry.currentStock = inv.has_value() ? inv->currentStock : 0;
            entry.status       = StockStatus::DEPLETED;
        } else if (inv->availableStock() <= 0) {
            entry.currentStock = inv->currentStock;
            entry.status       = StockStatus::SHORTAGE;
        } else {
            entry.currentStock = inv->currentStock;
            entry.status       = StockStatus::SUFFICIENT;
        }

        result.push_back(entry);
    }

    return result;
}
