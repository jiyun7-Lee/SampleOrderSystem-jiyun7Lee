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

    std::vector<Sample> FindByName(const std::string& keyword) const override {
        std::vector<Sample> result;
        for (const auto& pair : store_) {
            const auto& s = pair.second;
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

    // 인터페이스 계약 외 유틸리티 메서드 — 테스트 및 관리 목적으로만 사용
    void DeleteById(const std::string& id) {
        store_.erase(id);
    }

private:
    std::map<std::string, Sample> store_;
};
