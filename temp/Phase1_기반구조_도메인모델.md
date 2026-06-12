# Phase 1 — 프로젝트 기반 구조 및 도메인 모델

## 목표

MVC 스켈레톤, 도메인 모델, Repository 인터페이스, TimeProvider DI 구조를 구축한다.  
이후 모든 Phase의 기반이 되는 공통 타입과 인터페이스를 정의한다.

## 담당 에이전트

| 단계 | 에이전트 |
|------|---------|
| 테스트 작성 | `unit-test` |
| 구현 | `feature` |
| SOLID 검토 | `clean-code` |
| 빌드·실행 | `tester` |
| 커밋 | `git-manager` |

---

## Step 1-1: 디렉토리 구조 및 CMakeLists.txt 구성

### 작업 내용

- 프로젝트 루트에 `CMakeLists.txt` 생성
- `src/`, `tests/`, `temp/`, `docs/` 디렉토리 생성
- Google Test / Google Mock 연동 설정
- MSVC `/utf-8` 컴파일 옵션 설정

### CMakeLists.txt 핵심 설정

```cmake
cmake_minimum_required(VERSION 3.16)
project(SampleOrderSystem)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src)
add_subdirectory(tests)
```

### 완료 기준

- [ ] CMakeLists.txt 빌드 성공
- [ ] GTest 링크 확인

---

## Step 1-2: 주문 상태 `enum class OrderStatus` 정의

### 작업 내용

```cpp
// src/model/domain/OrderStatus.h
#pragma once

enum class OrderStatus {
    RESERVED,
    PRODUCING,
    CONFIRMED,
    RELEASE,
    REJECTED
};
```

### 완료 기준

- [ ] `OrderStatus` 타입 컴파일 통과
- [ ] 모든 상태값 정의 완료

---

## Step 1-3: 도메인 모델 클래스 구현

### Sample

```cpp
// src/model/domain/Sample.h
struct Sample {
    std::string sampleId;
    std::string name;
    double averageProductionTime; // 초/ea
    double yieldRate;             // 0.0 초과 ~ 1.0 이하
};
```

### Order

```cpp
// src/model/domain/Order.h
struct Order {
    std::string orderId;
    std::string sampleId;
    std::string customerName;
    int quantity;
    OrderStatus status;
    std::chrono::system_clock::time_point createdAt;
};
```

### Inventory

```cpp
// src/model/domain/Inventory.h
struct Inventory {
    std::string sampleId;
    int currentStock;   // 실제 생산 완료된 정상 시료 수량
    int reservedStock;  // 이미 주문이 확보한 수량

    int availableStock() const { return currentStock - reservedStock; }
};
```

### ProductionJob

```cpp
// src/model/domain/ProductionJob.h
struct ProductionJob {
    std::string productionId;
    std::string orderId;
    std::string sampleId;
    int requiredQuantity;
    int actualProductionQuantity;
    double totalProductionTime;   // 초
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point expectedFinishTime;
};
```

### 도메인 규칙 검증 (단위 테스트 대상)

| 테스트 케이스 | 조건 | 기대 결과 |
|-------------|------|---------|
| `YieldRate_Zero_ShouldBeInvalid` | yieldRate = 0 | 유효하지 않음 |
| `YieldRate_One_ShouldBeValid` | yieldRate = 1.0 | 유효함 |
| `AvailableStock_ReturnsCorrectValue` | currentStock=100, reservedStock=30 | availableStock=70 |
| `AvailableStock_CannotBeNegative` | currentStock=10, reservedStock=30 | 처리 규칙 확인 |

### 완료 기준

- [ ] 4개 도메인 클래스 정의 완료
- [ ] `availableStock()` 계산 정확

---

## Step 1-4: Repository 인터페이스 정의

```cpp
// src/model/repository/ISampleRepository.h
class ISampleRepository {
public:
    virtual ~ISampleRepository() = default;
    virtual void Save(const Sample& sample) = 0;
    virtual std::optional<Sample> FindById(const std::string& id) const = 0;
    virtual std::vector<Sample> FindAll() const = 0;
    virtual std::vector<Sample> FindByName(const std::string& name) const = 0;
    virtual bool ExistsById(const std::string& id) const = 0;
};

// IOrderRepository, IInventoryRepository, IProductionRepository 동일 패턴
```

### 완료 기준

- [ ] 4개 Repository 인터페이스 정의
- [ ] 순수 가상 함수로만 구성 (ISP 준수)

---

## Step 1-5: ITimeProvider 인터페이스 및 SystemTimeProvider 구현

```cpp
// src/util/ITimeProvider.h
class ITimeProvider {
public:
    virtual ~ITimeProvider() = default;
    virtual std::chrono::system_clock::time_point Now() const = 0;
};

// src/util/SystemTimeProvider.h
class SystemTimeProvider : public ITimeProvider {
public:
    std::chrono::system_clock::time_point Now() const override {
        return std::chrono::system_clock::now();
    }
};
```

### 완료 기준

- [ ] `ITimeProvider` 인터페이스 정의
- [ ] `SystemTimeProvider` 구현 완료
- [ ] 모든 서비스는 `ITimeProvider`를 통해 시간 조회 (직접 `now()` 호출 금지)

---

## Step 1-6: InMemory Repository 구현체

각 Repository 인터페이스의 InMemory 구현체를 `std::map` / `std::vector`로 구현한다.

```cpp
// src/model/repository/InMemorySampleRepository.h
class InMemorySampleRepository : public ISampleRepository {
    std::map<std::string, Sample> store_;
public:
    void Save(const Sample& sample) override { store_[sample.sampleId] = sample; }
    // ...
};
```

### 완료 기준

- [ ] 4개 InMemory Repository 구현
- [ ] CRUD 동작 확인

---

## Step 1-7: Main 진입점 및 콘솔 한글 인코딩 설정

```cpp
// src/main.cpp
#include <windows.h>
#include <iostream>

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // AppController 또는 MainMenu 진입
    return 0;
}
```

### 완료 기준

- [ ] 한글 콘솔 출력 정상 동작
- [ ] `chcp` 결과 65001 확인

---

## Phase 1 완료 기준

- [ ] 모든 Step(1-1 ~ 1-7) 완료
- [ ] 빌드 오류 없음
- [ ] 도메인 단위 테스트 PASS
- [ ] `[feat] Phase 1 — 기반 구조 및 도메인 모델` 커밋 완료
