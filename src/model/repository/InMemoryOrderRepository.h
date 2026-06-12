#pragma once

#include "IOrderRepository.h"
#include <map>

class InMemoryOrderRepository : public IOrderRepository {
public:
    void Save(const Order& order) override {
        store_[order.orderId] = order;
    }

    std::optional<Order> FindById(const std::string& id) const override {
        auto it = store_.find(id);
        if (it == store_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<Order> FindAll() const override {
        std::vector<Order> result;
        result.reserve(store_.size());
        for (const auto& pair : store_) {
            result.push_back(pair.second);
        }
        return result;
    }

    std::vector<Order> FindByStatus(OrderStatus status) const override {
        std::vector<Order> result;
        for (const auto& pair : store_) {
            if (pair.second.status == status) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    bool ExistsById(const std::string& id) const override {
        return store_.find(id) != store_.end();
    }

    void DeleteById(const std::string& id) override {
        store_.erase(id);
    }

private:
    std::map<std::string, Order> store_;
};
