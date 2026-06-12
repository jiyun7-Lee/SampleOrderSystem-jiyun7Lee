#pragma once
#include <vector>
#include <string>
#include "../model/repository/IOrderRepository.h"
#include "../model/repository/IInventoryRepository.h"
#include "../model/repository/ISampleRepository.h"

enum class StockStatus { SUFFICIENT, SHORTAGE, DEPLETED };

struct OrderStatusSummary {
    int reservedCount  = 0;
    int producingCount = 0;
    int confirmedCount = 0;
    int releaseCount   = 0;
};

struct InventoryStatus {
    std::string sampleId;
    std::string sampleName;
    int currentStock;
    StockStatus status;
};

class MonitoringService {
public:
    MonitoringService(IOrderRepository& orderRepo,
                      IInventoryRepository& inventoryRepo,
                      ISampleRepository& sampleRepo);
    OrderStatusSummary GetOrderSummary() const;
    std::vector<InventoryStatus> GetInventoryStatus() const;
private:
    IOrderRepository& orderRepo_;
    IInventoryRepository& inventoryRepo_;
    ISampleRepository& sampleRepo_;
};
