#pragma once

#include <string>
#include "../model/repository/IInventoryRepository.h"
#include "../model/domain/Inventory.h"

class InventoryService {
public:
    explicit InventoryService(IInventoryRepository& repo);

    int GetAvailableStock(const std::string& sampleId);
    void AllocateStock(const std::string& sampleId, int qty);
    void AddStock(const std::string& sampleId, int qty);
    void DeductStock(const std::string& sampleId, int qty);
    Inventory GetInventory(const std::string& sampleId);

private:
    IInventoryRepository& repo_;
};
