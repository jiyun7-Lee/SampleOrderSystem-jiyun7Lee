#include "InventoryService.h"
#include <stdexcept>

InventoryService::InventoryService(IInventoryRepository& repo)
    : repo_(repo) {}

int InventoryService::GetAvailableStock(const std::string& sampleId) {
    auto inv = repo_.FindBySampleId(sampleId);
    if (!inv.has_value()) {
        throw std::runtime_error("Inventory not found for sampleId: " + sampleId);
    }
    return inv->availableStock();
}

void InventoryService::AllocateStock(const std::string& sampleId, int qty) {
    if (qty <= 0) {
        throw std::invalid_argument("qty must be greater than 0");
    }
    auto inv = repo_.FindBySampleId(sampleId);
    if (!inv.has_value()) {
        throw std::runtime_error("Inventory not found for sampleId: " + sampleId);
    }
    if (inv->availableStock() < qty) {
        throw std::runtime_error("Insufficient available stock for sampleId: " + sampleId);
    }
    inv->reservedStock += qty;
    repo_.Save(*inv);
}

void InventoryService::AddStock(const std::string& sampleId, int qty) {
    if (qty <= 0) {
        throw std::invalid_argument("qty must be greater than 0");
    }
    auto inv = repo_.FindBySampleId(sampleId);
    if (!inv.has_value()) {
        throw std::runtime_error("Inventory not found for sampleId: " + sampleId);
    }
    inv->currentStock += qty;
    repo_.Save(*inv);
}

void InventoryService::DeductStock(const std::string& sampleId, int qty) {
    if (qty <= 0) {
        throw std::invalid_argument("qty must be greater than 0");
    }
    auto inv = repo_.FindBySampleId(sampleId);
    if (!inv.has_value()) {
        throw std::runtime_error("Inventory not found for sampleId: " + sampleId);
    }
    if (qty > inv->currentStock) {
        throw std::runtime_error("Insufficient current stock for sampleId: " + sampleId);
    }
    inv->currentStock -= qty;
    repo_.Save(*inv);
}

Inventory InventoryService::GetInventory(const std::string& sampleId) {
    auto inv = repo_.FindBySampleId(sampleId);
    if (!inv.has_value()) {
        throw std::runtime_error("Inventory not found for sampleId: " + sampleId);
    }
    return *inv;
}
