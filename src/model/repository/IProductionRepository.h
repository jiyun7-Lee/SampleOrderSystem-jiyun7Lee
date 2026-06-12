#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../domain/ProductionJob.h"

// OCP/DIP: Phase 4 생산라인 조회 및 Phase 5 모니터링에서 전체 생산 작업 목록이
// 필요하다. FindAll()을 인터페이스에 포함하여 서비스 계층이 구체 타입에 의존하지
// 않고 추상화를 통해 전체 목록을 조회할 수 있게 한다.
class IProductionRepository {
public:
    virtual ~IProductionRepository() = default;

    virtual void Save(const ProductionJob& job) = 0;
    virtual std::optional<ProductionJob> FindById(const std::string& id) const = 0;
    virtual std::vector<ProductionJob> FindAll() const = 0;
    virtual std::vector<ProductionJob> FindByOrderId(const std::string& orderId) const = 0;
};
