#pragma once
#include <memory>
#include <vector>
#include <optional>
#include <string>
#include "../model/repository/ISampleRepository.h"
#include "../model/domain/Sample.h"

class SampleService {
public:
    explicit SampleService(std::shared_ptr<ISampleRepository> repo);

    void RegisterSample(const Sample& sample);
    std::vector<Sample> GetAllSamples() const;
    std::optional<Sample> FindSampleById(const std::string& id) const;
    std::vector<Sample> SearchByName(const std::string& name) const;

private:
    std::shared_ptr<ISampleRepository> _repo;

    void ValidateSample(const Sample& sample) const;
};
