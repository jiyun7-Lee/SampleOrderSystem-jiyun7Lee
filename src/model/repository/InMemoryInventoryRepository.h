#pragma once

#include "IInventoryRepository.h"
#include <map>

class InMemoryInventoryRepository : public IInventoryRepository {
public:
    void Save(const Inventory& inventory) override {
        store_[inventory.sampleId] = inventory;
    }

    std::optional<Inventory> FindBySampleId(const std::string& sampleId) const override {
        auto it = store_.find(sampleId);
        if (it == store_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    bool ExistsBySampleId(const std::string& sampleId) const override {
        return store_.find(sampleId) != store_.end();
    }

private:
    std::map<std::string, Inventory> store_;
};
