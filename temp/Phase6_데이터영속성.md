# Phase 6 — 데이터 영속성 (JSON 파일 기반)

## 목표

애플리케이션을 재시작해도 데이터가 유지되도록 JSON 파일 기반 영속성 계층을 구현한다.  
InMemory Repository를 JsonRepository로 교체하되, 비즈니스 로직은 변경하지 않는다.

## 담당 에이전트

| 단계 | 에이전트 |
|------|---------|
| 테스트 작성 | `unit-test` |
| 구현 | `feature` |
| SOLID 검토 | `clean-code` |
| 빌드·실행 | `tester` |
| 커밋 | `git-manager` |

---

## Step 6-1: JSON 라이브러리 연동 (nlohmann/json)

### CMakeLists.txt 추가

```cmake
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

target_link_libraries(SampleOrderSystem PRIVATE nlohmann_json::nlohmann_json)
```

### 저장 경로 구조

```
data/
├── samples.json
├── orders.json
├── inventory.json
└── production_queue.json
```

### 완료 기준

- [ ] nlohmann/json 빌드 연동 확인
- [ ] `data/` 디렉토리 자동 생성 로직

---

## Step 6-2: SampleJsonRepository 구현

### 직렬화 / 역직렬화

```cpp
// src/persistence/SampleJsonRepository.h
class SampleJsonRepository : public ISampleRepository {
    std::string filePath_;
    std::map<std::string, Sample> cache_;

    void Load();  // 파일 → cache_
    void Save();  // cache_ → 파일

public:
    void Save(const Sample& sample) override;
    std::optional<Sample> FindById(const std::string& id) const override;
    std::vector<Sample> FindAll() const override;
    std::vector<Sample> FindByName(const std::string& name) const override;
    bool ExistsById(const std::string& id) const override;
};
```

### JSON 형식

```json
[
  {
    "sampleId": "S-001",
    "name": "실리콘 웨이퍼-8인치",
    "averageProductionTime": 0.5,
    "yieldRate": 0.92
  }
]
```

### 단위 테스트 케이스

```cpp
TEST(SampleJsonRepositoryTest, Save_ShouldPersistToFile)
TEST(SampleJsonRepositoryTest, Load_ShouldRestoreFromFile)
TEST(SampleJsonRepositoryTest, FindById_AfterSave_ShouldReturnSample)
TEST(SampleJsonRepositoryTest, FindAll_ShouldReturnAllSaved)
```

### 완료 기준

- [ ] SampleJsonRepository 구현 완료
- [ ] 파일 저장·로드 정확
- [ ] 단위 테스트 PASS

---

## Step 6-3: OrderJsonRepository 구현

### JSON 형식

```json
[
  {
    "orderId": "ORD-20260612-0001",
    "sampleId": "S-001",
    "customerName": "SK하이닉스",
    "quantity": 150,
    "status": "CONFIRMED",
    "createdAt": "2026-06-12T09:30:00"
  }
]
```

### 완료 기준

- [ ] OrderJsonRepository 구현 완료
- [ ] OrderStatus enum ↔ string 변환 정확
- [ ] createdAt 직렬화/역직렬화 정확

---

## Step 6-4: InventoryJsonRepository 구현

### JSON 형식

```json
[
  {
    "sampleId": "S-001",
    "currentStock": 480,
    "reservedStock": 150
  }
]
```

### 완료 기준

- [ ] InventoryJsonRepository 구현 완료
- [ ] currentStock / reservedStock 정확 저장

---

## Step 6-5: ProductionJobJsonRepository 구현

### JSON 형식

```json
[
  {
    "productionId": "PROD-0001",
    "orderId": "ORD-20260612-0003",
    "sampleId": "S-003",
    "requiredQuantity": 170,
    "actualProductionQuantity": 206,
    "totalProductionTime": 164.8,
    "startTime": "2026-06-12T09:00:00",
    "expectedFinishTime": "2026-06-12T11:44:48"
  }
]
```

### 완료 기준

- [ ] ProductionJobJsonRepository 구현 완료
- [ ] startTime / expectedFinishTime 직렬화/역직렬화 정확

---

## Step 6-6: 애플리케이션 시작 시 로드 / 변경 시 자동 저장

### 초기화 흐름

```
main()
  → JsonRepository 인스턴스 생성 (data/*.json 로드)
  → Service에 DI 주입
  → 메인 메뉴 시작
```

### 저장 전략

- 각 Repository의 `Save()` 호출 시 파일 즉시 저장 (Write-Through)
- 애플리케이션 종료 전 추가 저장 불필요

### 완료 기준

- [ ] 앱 재시작 후 이전 데이터 정상 로드
- [ ] 시료 등록 → 종료 → 재시작 → 목록에서 확인

---

## Phase 6 완료 기준

- [ ] 4개 JsonRepository 구현 완료 (Sample, Order, Inventory, ProductionJob)
- [ ] 애플리케이션 재시작 후 데이터 유지 확인
- [ ] 기존 비즈니스 로직 변경 없음
- [ ] 단위 테스트 PASS
- [ ] 커버리지 측정 완료
- [ ] `[feat] Phase 6 — 데이터 영속성` 커밋 완료
