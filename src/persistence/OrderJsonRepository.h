#pragma once

#include "../model/repository/IOrderRepository.h"
#include "../model/domain/OrderStatus.h"
#include "../third_party/nlohmann/json.hpp"
#include "TimePointSerializer.h"
#include <fstream>
#include <map>
#include <filesystem>
#include <stdexcept>

class OrderJsonRepository : public IOrderRepository {
public:
    explicit OrderJsonRepository(const std::filesystem::path& filePath)
        : filePath_(filePath) {
        Load();
    }

    void Save(const Order& order) override {
        store_[order.orderId] = order;
        Persist();
    }

    std::optional<Order> FindById(const std::string& id) const override {
        auto it = store_.find(id);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<Order> FindAll() const override {
        std::vector<Order> result;
        result.reserve(store_.size());
        for (const auto& [id, o] : store_) result.push_back(o);
        return result;
    }

    std::vector<Order> FindByStatus(OrderStatus status) const override {
        std::vector<Order> result;
        for (const auto& [id, o] : store_) {
            if (o.status == status) result.push_back(o);
        }
        return result;
    }

    bool ExistsById(const std::string& id) const override {
        return store_.find(id) != store_.end();
    }

private:
    std::filesystem::path filePath_;
    std::map<std::string, Order> store_;

    [[nodiscard]] static std::string StatusToString(OrderStatus s) {
        switch (s) {
            case OrderStatus::RESERVED:  return "RESERVED";
            case OrderStatus::PRODUCING: return "PRODUCING";
            case OrderStatus::CONFIRMED: return "CONFIRMED";
            case OrderStatus::RELEASE:   return "RELEASE";
            case OrderStatus::REJECTED:  return "REJECTED";
            default:                     return "RESERVED";
        }
    }

    [[nodiscard]] static OrderStatus StringToStatus(const std::string& s) {
        if (s == "PRODUCING") return OrderStatus::PRODUCING;
        if (s == "CONFIRMED") return OrderStatus::CONFIRMED;
        if (s == "RELEASE")   return OrderStatus::RELEASE;
        if (s == "REJECTED")  return OrderStatus::REJECTED;
        return OrderStatus::RESERVED;
    }

    void Load() {
        std::ifstream ifs(filePath_);
        if (!ifs.is_open()) return;
        try {
            nlohmann::json j;
            ifs >> j;
            for (const auto& item : j) {
                Order o;
                o.orderId      = item.at("orderId").get<std::string>();
                o.sampleId     = item.at("sampleId").get<std::string>();
                o.customerName = item.at("customerName").get<std::string>();
                o.quantity     = item.at("quantity").get<int>();
                o.status       = StringToStatus(item.at("status").get<std::string>());
                o.createdAt    = TimePointSerializer::FromEpoch(item.at("createdAt").get<int64_t>());
                store_[o.orderId] = o;
            }
        } catch (const std::exception&) {
            store_.clear();
        }
    }

    void Persist() const {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& [id, o] : store_) {
            j.push_back({
                {"orderId",      o.orderId},
                {"sampleId",     o.sampleId},
                {"customerName", o.customerName},
                {"quantity",     o.quantity},
                {"status",       StatusToString(o.status)},
                {"createdAt",    TimePointSerializer::ToEpoch(o.createdAt)}
            });
        }
        std::ofstream ofs(filePath_);
        if (!ofs) throw std::runtime_error("OrderJsonRepository: failed to open file for writing: " + filePath_.string());
        ofs << j.dump(2);
    }
};
