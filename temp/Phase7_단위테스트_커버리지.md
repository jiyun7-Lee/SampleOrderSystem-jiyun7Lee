# Phase 7 — 단위 테스트 및 커버리지 달성

## 목표

Google Mock(gmock)을 활용하여 모든 서비스·도메인 레이어의 단위 테스트를 완성하고,  
코드 커버리지 **80% 이상**을 달성한다.

## 담당 에이전트

| 단계 | 에이전트 |
|------|---------|
| 테스트 작성 | `unit-test` |
| 커버리지 측정 | `coverage` |
| 빌드·실행 | `tester` |
| 커밋 | `git-manager` |
| 문서화 | `docs-writer` |

---

## gmock 기반 테스트 원칙

- 모든 의존성은 **인터페이스 + gmock**으로 주입 (실제 구현체 사용 금지)
- `std::chrono::system_clock::now()` 직접 호출 금지 — `MockTimeProvider` 사용
- 실제 파일 I/O 사용 금지 — `MockRepository` 사용
- 실제 시간을 기다리는 테스트 금지

---

## Step 7-1: Domain 단위 테스트

### 테스트 대상

| 클래스 | 테스트 항목 |
|--------|------------|
| `Sample` | yieldRate 범위 검증, averageProductionTime 양수 검증 |
| `Order` | 상태 초기값, orderId 형식 |
| `Inventory` | `availableStock()` 계산, 음수 방어 |
| `ProductionJob` | 생산량 계산, 생산시간 계산 |

### 테스트 파일

```
tests/domain/SampleTest.cpp
tests/domain/OrderTest.cpp
tests/domain/InventoryTest.cpp
tests/domain/ProductionJobTest.cpp
```

### 테스트 케이스

```cpp
// InventoryTest
TEST(InventoryTest, AvailableStock_Normal_ShouldReturnDifference)
TEST(InventoryTest, AvailableStock_ReservedExceedsCurrent_ShouldReturnZero)

// ProductionJobTest
TEST(ProductionJobTest, ActualQty_Standard_ShouldBeCeil)
TEST(ProductionJobTest, ActualQty_LowYield_ShouldBeHigherQty)
TEST(ProductionJobTest, ActualQty_Example_100_01_ShouldBe1112)
TEST(ProductionJobTest, TotalTime_3sec_100ea_ShouldBe300sec)
```

### 완료 기준

- [ ] Domain 테스트 전체 PASS
- [ ] 경계값 케이스 포함

---

## Step 7-2: SampleService 단위 테스트

### Mock 구성

```cpp
class MockSampleRepository : public ISampleRepository {
public:
    MOCK_METHOD(void, Save, (const Sample&), (override));
    MOCK_METHOD(std::optional<Sample>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindByName, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};
```

### 테스트 케이스

```cpp
TEST(SampleServiceTest, Register_DuplicateId_ShouldThrow)
TEST(SampleServiceTest, Register_EmptyName_ShouldThrow)
TEST(SampleServiceTest, Register_ZeroProductionTime_ShouldThrow)
TEST(SampleServiceTest, Register_YieldRateZero_ShouldThrow)
TEST(SampleServiceTest, Register_YieldRateOverOne_ShouldThrow)
TEST(SampleServiceTest, Register_ValidInput_ShouldCallSave)
TEST(SampleServiceTest, GetAll_ShouldReturnAll)
TEST(SampleServiceTest, SearchByName_ShouldReturnMatching)
TEST(SampleServiceTest, SearchByName_NoMatch_ShouldReturnEmpty)
```

### 완료 기준

- [ ] SampleService 테스트 전체 PASS

---

## Step 7-3: OrderService 단위 테스트

### Mock 구성

```cpp
class MockOrderRepository : public IOrderRepository {
public:
    MOCK_METHOD(void, Save, (const Order&), (override));
    MOCK_METHOD(std::optional<Order>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Order>, FindByStatus, (OrderStatus), (const, override));
};

class MockInventoryRepository : public IInventoryRepository {
public:
    MOCK_METHOD(std::optional<Inventory>, FindBySampleId, (const std::string&), (const, override));
    MOCK_METHOD(void, Save, (const Inventory&), (override));
};

class MockProductionRepository : public IProductionRepository {
public:
    MOCK_METHOD(void, Enqueue, (const ProductionJob&), (override));
};

class MockTimeProvider : public ITimeProvider {
public:
    MOCK_METHOD(std::chrono::system_clock::time_point, Now, (), (const, override));
};
```

### 테스트 케이스

```cpp
// 주문 접수
TEST(OrderServiceTest, PlaceOrder_ValidInput_ShouldBeReserved)
TEST(OrderServiceTest, PlaceOrder_InvalidSampleId_ShouldThrow)
TEST(OrderServiceTest, PlaceOrder_ZeroQuantity_ShouldThrow)

// 주문 승인 — 재고 충분
TEST(OrderServiceTest, Approve_SufficientStock_ShouldBeConfirmed)
TEST(OrderServiceTest, Approve_SufficientStock_ShouldAllocateReservedStock)
TEST(OrderServiceTest, Approve_SufficientStock_ShouldNotCreateProductionJob)

// 주문 승인 — 재고 부족
TEST(OrderServiceTest, Approve_InsufficientStock_ShouldBeProducing)
TEST(OrderServiceTest, Approve_InsufficientStock_ShouldCreateProductionJob)
TEST(OrderServiceTest, Approve_InsufficientStock_RequiredQtyCalculatedOnce)

// 주문 거절
TEST(OrderServiceTest, Reject_Reserved_ShouldBeRejected)
TEST(OrderServiceTest, Reject_NotReserved_ShouldThrow)
TEST(OrderServiceTest, Approve_NotReserved_ShouldThrow)
```

### 완료 기준

- [ ] OrderService 테스트 전체 PASS
- [ ] 승인 분기(CONFIRMED/PRODUCING) 모두 테스트

---

## Step 7-4: ProductionService 단위 테스트

### 시간 시뮬레이션 전략

```cpp
// MockTimeProvider로 시간 조작
auto fakeNow = std::chrono::system_clock::from_time_t(1000);
auto fakeFinish = fakeNow + std::chrono::seconds(300);

EXPECT_CALL(mockTime, Now())
    .WillOnce(Return(fakeNow))       // 생산 시작 시각
    .WillOnce(Return(fakeFinish + std::chrono::seconds(1))); // 완료 이후
```

### 테스트 케이스

```cpp
TEST(ProductionServiceTest, CheckCompletion_BeforeFinish_ShouldNotComplete)
TEST(ProductionServiceTest, CheckCompletion_AfterFinish_ShouldComplete)
TEST(ProductionServiceTest, OnComplete_ShouldChangeOrderToConfirmed)
TEST(ProductionServiceTest, OnComplete_ShouldAddStockToInventory)
TEST(ProductionServiceTest, OnComplete_ShouldDequeueJob)
TEST(ProductionServiceTest, OnComplete_ShouldStartNextJobIfExists)
TEST(ProductionServiceTest, ExcessProduction_ShouldBeAvailableImmediately)
TEST(ProductionServiceTest, FIFO_MultipleJobs_ProcessedInOrder)
```

### 완료 기준

- [ ] ProductionService 테스트 전체 PASS
- [ ] 시간 시뮬레이션 테스트 포함

---

## Step 7-5: MonitoringService / ReleaseService 단위 테스트

### 테스트 케이스

```cpp
// MonitoringService
TEST(MonitoringServiceTest, OrderSummary_ExcludesRejected)
TEST(MonitoringServiceTest, OrderSummary_CountsByStatus)
TEST(MonitoringServiceTest, InventoryStatus_ZeroStock_Depleted)
TEST(MonitoringServiceTest, InventoryStatus_NoAvailable_Shortage)
TEST(MonitoringServiceTest, InventoryStatus_HasAvailable_Sufficient)

// ReleaseService
TEST(ReleaseServiceTest, GetConfirmedOrders_OnlyConfirmed)
TEST(ReleaseServiceTest, Release_ShouldChangeToRelease)
TEST(ReleaseServiceTest, Release_ShouldDeductCurrentStock)
TEST(ReleaseServiceTest, Release_ShouldDeallocateReservedStock)
TEST(ReleaseServiceTest, Release_NotConfirmed_ShouldThrow)
```

### 완료 기준

- [ ] 전체 테스트 PASS

---

## Step 7-6: 커버리지 측정 (OpenCppCoverage)

### 측정 대상

- `src/service/` — SampleService, OrderService, InventoryService, ProductionService, MonitoringService, ReleaseService
- `src/model/domain/` — Sample, Order, Inventory, ProductionJob
- `src/model/repository/` — InMemory 구현체
- `src/persistence/` — JsonRepository 구현체

### 목표

| 레이어 | 목표 커버리지 |
|--------|-------------|
| Domain | 90% 이상 |
| Service | 85% 이상 |
| Repository | 80% 이상 |
| **전체** | **80% 이상** |

### 측정 명령

```powershell
OpenCppCoverage.exe --sources src\ -- tests\Debug\SampleOrderSystemTests.exe
```

### 완료 기준

- [ ] 전체 커버리지 80% 이상
- [ ] `temp/COVERAGE.md` 결과 기록
- [ ] 미커버 구간 식별 및 추가 테스트 작성

---

## Phase 7 완료 기준

- [ ] Domain 단위 테스트 전체 PASS
- [ ] Service 단위 테스트 전체 PASS (gmock DI)
- [ ] MockTimeProvider로 시간 시뮬레이션 테스트 포함
- [ ] 커버리지 80% 이상 달성
- [ ] `temp/COVERAGE.md` 결과 기록
- [ ] `docs/TEST_LIST.md` 전체 케이스 ✅ PASS 갱신
- [ ] `[feat] Phase 7 — 단위 테스트 및 커버리지` 커밋 완료
- [ ] `temp/` 폴더 git 추적 해제 (`git rm -r --cached temp/`)
