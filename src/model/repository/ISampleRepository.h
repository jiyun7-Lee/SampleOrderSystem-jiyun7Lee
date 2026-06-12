#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../domain/Sample.h"

class ISampleRepository {
public:
    virtual ~ISampleRepository() = default;

    virtual void Save(const Sample& sample) = 0;
    virtual std::optional<Sample> FindById(const std::string& id) const = 0;
    virtual std::vector<Sample> FindAll() const = 0;
    virtual std::vector<Sample> FindByName(const std::string& name) const = 0;
    virtual bool ExistsById(const std::string& id) const = 0;
    virtual void DeleteById(const std::string& id) = 0;
};
