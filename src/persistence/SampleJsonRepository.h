#pragma once

#include "../model/repository/ISampleRepository.h"
#include "../third_party/nlohmann/json.hpp"
#include <fstream>
#include <map>

class SampleJsonRepository : public ISampleRepository {
public:
    explicit SampleJsonRepository(const std::string& filePath)
        : filePath_(filePath) {
        Load();
    }

    void Save(const Sample& sample) override {
        store_[sample.sampleId] = sample;
        Persist();
    }

    std::optional<Sample> FindById(const std::string& id) const override {
        auto it = store_.find(id);
        if (it == store_.end()) return std::nullopt;
        return it->second;
    }

    std::vector<Sample> FindAll() const override {
        std::vector<Sample> result;
        result.reserve(store_.size());
        for (const auto& [id, s] : store_) result.push_back(s);
        return result;
    }

    std::vector<Sample> FindByName(const std::string& keyword) const override {
        std::vector<Sample> result;
        for (const auto& [id, s] : store_) {
            if (s.name.find(keyword) != std::string::npos ||
                s.sampleId.find(keyword) != std::string::npos) {
                result.push_back(s);
            }
        }
        return result;
    }

    bool ExistsById(const std::string& id) const override {
        return store_.find(id) != store_.end();
    }

private:
    std::string filePath_;
    std::map<std::string, Sample> store_;

    void Load() {
        std::ifstream ifs(filePath_);
        if (!ifs.is_open()) return;
        try {
            nlohmann::json j;
            ifs >> j;
            for (const auto& item : j) {
                Sample s;
                s.sampleId              = item.at("sampleId").get<std::string>();
                s.name                  = item.at("name").get<std::string>();
                s.averageProductionTime = item.at("averageProductionTime").get<double>();
                s.yieldRate             = item.at("yieldRate").get<double>();
                store_[s.sampleId]      = s;
            }
        } catch (...) {}
    }

    void Persist() const {
        nlohmann::json j = nlohmann::json::array();
        for (const auto& [id, s] : store_) {
            j.push_back({
                {"sampleId",              s.sampleId},
                {"name",                  s.name},
                {"averageProductionTime", s.averageProductionTime},
                {"yieldRate",             s.yieldRate}
            });
        }
        std::ofstream ofs(filePath_);
        ofs << j.dump(2);
    }
};
