#pragma once

#include "IProductionRepository.h"
#include <map>

class InMemoryProductionRepository : public IProductionRepository {
public:
    void Save(const ProductionJob& job) override {
        store_[job.productionId] = job;
    }

    std::optional<ProductionJob> FindById(const std::string& id) const override {
        auto it = store_.find(id);
        if (it == store_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<ProductionJob> FindAll() const override {
        std::vector<ProductionJob> result;
        result.reserve(store_.size());
        for (const auto& pair : store_) {
            result.push_back(pair.second);
        }
        return result;
    }

    std::vector<ProductionJob> FindByOrderId(const std::string& orderId) const override {
        std::vector<ProductionJob> result;
        for (const auto& pair : store_) {
            if (pair.second.orderId == orderId) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

private:
    std::map<std::string, ProductionJob> store_;
};
