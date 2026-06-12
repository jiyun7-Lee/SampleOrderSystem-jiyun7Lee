#pragma once

#include "../model/repository/IInventoryRepository.h"
#include "../third_party/nlohmann/json.hpp"
#include <fstream>
#include <map>
#include <filesystem>
#include <stdexcept>

class InventoryJsonRepository : public IInventoryRepository {
public:
    explicit InventoryJsonRepository(const std::filesystem::path& filePath)
        : filePath_(filePath) {
        Load();
    }

    void Save(const Inventory& inv) override {
        store_[inv.sampleId] = inv;
        Persist();
    }

    std::optional<Inventory> FindBySampleId(const std::string& sampleId) const override {
        auto it = store_.find(sampleId);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    bool ExistsBySampleId(const std::string& sampleId) const override {
        return store_.find(sampleId) != store_.end();
    }

private:
    std::filesystem::path filePath_;
    std::map<std::string, Inventory> store_;

    void Load() {
        std::ifstream ifs(filePath_);
        if (!ifs.is_open()) return;
        try {
            nlohmann::json j;
            ifs >> j;
            for (const auto& item : j) {
                Inventory inv;
                inv.sampleId      = item.at("sampleId").get<std::string>();
                inv.currentStock  = item.at("currentStock").get<int>();
                inv.reservedStock = item.at("reservedStock").get<int>();
                store_[inv.sampleId] = inv;
            }
        } catch (const std::exception&) {
            store_.clear();
        }
    }

    void Persist() const {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& [id, inv] : store_) {
            j.push_back({
                {"sampleId",      inv.sampleId},
                {"currentStock",  inv.currentStock},
                {"reservedStock", inv.reservedStock}
            });
        }
        std::ofstream ofs(filePath_);
        if (!ofs) throw std::runtime_error("InventoryJsonRepository: failed to open file for writing: " + filePath_.string());
        ofs << j.dump(2);
    }
};
