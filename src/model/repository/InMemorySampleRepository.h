#pragma once

#include "ISampleRepository.h"
#include <map>
#include <algorithm>

class InMemorySampleRepository : public ISampleRepository {
public:
    void Save(const Sample& sample) override {
        store_[sample.sampleId] = sample;
    }

    std::optional<Sample> FindById(const std::string& id) const override {
        auto it = store_.find(id);
        if (it == store_.end()) {
            return std::nullopt;
        }
        return it->second;
    }

    std::vector<Sample> FindAll() const override {
        std::vector<Sample> result;
        result.reserve(store_.size());
        for (const auto& pair : store_) {
            result.push_back(pair.second);
        }
        return result;
    }

    std::vector<Sample> FindByName(const std::string& name) const override {
        std::vector<Sample> result;
        for (const auto& pair : store_) {
            if (pair.second.name == name) {
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
    std::map<std::string, Sample> store_;
};
