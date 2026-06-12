#pragma once

#include "../model/repository/IProductionRepository.h"
#include "../third_party/nlohmann/json.hpp"
#include <fstream>
#include <map>
#include <chrono>

class ProductionJobJsonRepository : public IProductionRepository {
public:
    explicit ProductionJobJsonRepository(const std::string& filePath)
        : filePath_(filePath) {
        Load();
    }

    void Save(const ProductionJob& job) override {
        store_[job.productionId] = job;
        Persist();
    }

    std::optional<ProductionJob> FindById(const std::string& id) const override {
        auto it = store_.find(id);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<ProductionJob> FindAll() const override {
        std::vector<ProductionJob> result;
        result.reserve(store_.size());
        for (const auto& [id, job] : store_) result.push_back(job);
        return result;
    }

    std::vector<ProductionJob> FindByOrderId(const std::string& orderId) const override {
        std::vector<ProductionJob> result;
        for (const auto& [id, job] : store_) {
            if (job.orderId == orderId) result.push_back(job);
        }
        return result;
    }

private:
    std::string filePath_;
    std::map<std::string, ProductionJob> store_;

    static int64_t ToEpoch(const std::chrono::system_clock::time_point& tp) {
        return std::chrono::duration_cast<std::chrono::seconds>(
            tp.time_since_epoch()).count();
    }

    static std::chrono::system_clock::time_point FromEpoch(int64_t sec) {
        return std::chrono::system_clock::time_point(std::chrono::seconds(sec));
    }

    void Load() {
        std::ifstream ifs(filePath_);
        if (!ifs.is_open()) return;
        try {
            nlohmann::json j;
            ifs >> j;
            for (const auto& item : j) {
                ProductionJob job;
                job.productionId             = item.at("productionId").get<std::string>();
                job.orderId                  = item.at("orderId").get<std::string>();
                job.sampleId                 = item.at("sampleId").get<std::string>();
                job.requiredQuantity         = item.at("requiredQuantity").get<int>();
                job.actualProductionQuantity = item.at("actualProductionQuantity").get<int>();
                job.totalProductionTime      = item.at("totalProductionTime").get<double>();
                job.startTime               = FromEpoch(item.at("startTime").get<int64_t>());
                job.expectedFinishTime      = FromEpoch(item.at("expectedFinishTime").get<int64_t>());
                store_[job.productionId] = job;
            }
        } catch (...) {}
    }

    void Persist() const {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& [id, job] : store_) {
            j.push_back({
                {"productionId",             job.productionId},
                {"orderId",                  job.orderId},
                {"sampleId",                 job.sampleId},
                {"requiredQuantity",         job.requiredQuantity},
                {"actualProductionQuantity", job.actualProductionQuantity},
                {"totalProductionTime",      job.totalProductionTime},
                {"startTime",               ToEpoch(job.startTime)},
                {"expectedFinishTime",      ToEpoch(job.expectedFinishTime)}
            });
        }
        std::ofstream ofs(filePath_);
        ofs << j.dump(2);
    }
};
