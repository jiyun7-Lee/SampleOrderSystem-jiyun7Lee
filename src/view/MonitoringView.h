#pragma once
#include <vector>
#include <string>
#include "../service/MonitoringService.h"

class MonitoringView {
public:
    void ShowMenu() const;
    void ShowOrderSummary(const OrderStatusSummary& summary) const;
    void ShowInventoryStatus(const std::vector<InventoryStatus>& statuses) const;
    void ShowMessage(const std::string& msg) const;
    int InputInt(const std::string& prompt) const;

private:
    static std::string StockStatusToString(StockStatus status);
};
