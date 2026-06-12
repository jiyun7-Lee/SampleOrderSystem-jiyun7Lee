#include "SampleService.h"
#include <stdexcept>

SampleService::SampleService(std::shared_ptr<ISampleRepository> repo)
    : _repo(std::move(repo)) {}

void SampleService::RegisterSample(const Sample& sample) {
    if (sample.sampleId.empty()) {
        throw std::invalid_argument("시료 ID는 비어있을 수 없습니다.");
    }
    if (sample.yieldRate < 0.0 || sample.yieldRate > 1.0) {
        throw std::invalid_argument("수율은 0.0 이상 1.0 이하이어야 합니다.");
    }
    if (sample.averageProductionTime < 0.0) {
        throw std::invalid_argument("평균 생산시간은 0 이상이어야 합니다.");
    }
    if (_repo->ExistsById(sample.sampleId)) {
        throw std::runtime_error("이미 존재하는 시료 ID입니다: " + sample.sampleId);
    }
    _repo->Save(sample);
}

std::vector<Sample> SampleService::GetAllSamples() const {
    return _repo->FindAll();
}

std::optional<Sample> SampleService::FindSampleById(const std::string& id) const {
    return _repo->FindById(id);
}

std::vector<Sample> SampleService::SearchByName(const std::string& name) const {
    return _repo->FindByName(name);
}
