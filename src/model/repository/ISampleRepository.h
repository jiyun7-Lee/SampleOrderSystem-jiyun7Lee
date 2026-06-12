#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../domain/Sample.h"

// ISP: 시료 도메인은 삭제 시나리오가 없으므로 DeleteById를 인터페이스에서 제거.
// 삭제가 필요한 구현체는 구체 클래스 수준에서 별도 제공한다.
class ISampleRepository {
public:
    virtual ~ISampleRepository() = default;

    virtual void Save(const Sample& sample) = 0;
    virtual std::optional<Sample> FindById(const std::string& id) const = 0;
    virtual std::vector<Sample> FindAll() const = 0;
    virtual std::vector<Sample> FindByName(const std::string& name) const = 0;
    virtual bool ExistsById(const std::string& id) const = 0;
};
