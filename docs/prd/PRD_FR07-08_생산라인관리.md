# PRD — FR-07~08. 생산라인 관리

> 상위 문서: [PRD.md](../../PRD.md) | 관련 Phase: [Phase4_생산라인관리.md](../../temp/Phase4_생산라인관리.md)

---

## 생산라인 구조

- 생산라인은 **1개만** 존재한다.
- 동시에 **하나의 생산 작업만** 수행한다.
- 생산 작업은 **FIFO(선입선출)** 방식으로 순차 처리한다.
- 생산은 **실시간으로 진행**된다.
- 생산 시작 시간부터 총 생산 시간이 경과할 때까지 생산 작업 정보가 유지된다.

---

## FR-07 생산 큐 관리

### 기능

- `PRODUCING` 전환 시 `ProductionJob`을 FIFO 큐에 자동 등록
- 생산 큐 목록 조회 가능 (대기 중인 작업 순서 확인)

### 데이터 구조

```cpp
std::queue<ProductionJob>  // FIFO 보장
```

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(ProductionServiceTest, EnqueueJob_ShouldAddToQueue)
TEST(ProductionServiceTest, GetQueue_ShouldReturnFifoOrder)

// Abnormal
TEST(ProductionServiceTest, GetQueue_Empty_ShouldReturnEmptyList)
```

---

## FR-08 생산 처리

### 처리 흐름

1. 생산 큐에서 첫 번째 작업 선택 (FIFO)
2. 부족 수량으로 실 생산량 계산
3. 총 생산 시간 계산
4. 생산 완료 시 → 재고 증가 + 주문 상태 `PRODUCING` → `CONFIRMED`

---

### 생산량 계산 공식

수율 및 오차(10%)를 고려하여 실 생산량을 계산한다.

```
actualProductionQuantity = ceil(requiredQuantity / (yieldRate * 0.9))
```

**예시**

```
requiredQuantity = 100
yieldRate        = 0.1

actualProductionQuantity = ceil(100 / (0.1 * 0.9)) = ceil(1111.11) = 1112
```

> 초과 생산된 수량도 모두 **정상 시료로 간주**한다.

---

### 생산 시간 계산 공식

```
totalProductionTime = averageProductionTime(초) * actualProductionQuantity
```

**예시**

```
averageProductionTime    = 3초
actualProductionQuantity = 100

totalProductionTime = 300초
```

---

### 초과 생산분 활용 규칙

생산 과정에서 발생한 초과 생산분은 **즉시 가용 재고(availableStock)로 반영**한다.

**예시**

```
주문1 수량       = 100
실생산량         = 120
현재 생산 완료   = 115

주문1 확보 수량  = 100  (reservedStock)
즉시 가용 수량   = 15   (availableStock — 신규 주문에 즉시 사용 가능)
```

---

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(ProductionServiceTest, ProcessJob_ShouldCompleteAndConfirm)
TEST(ProductionServiceTest, CalculateActualQty_NormalYield_ShouldCeil)
TEST(ProductionServiceTest, CalculateActualQty_LowYield_ShouldProduceLarge)
TEST(ProductionServiceTest, ExcessProduction_ShouldIncreaseAvailableStock)

// Abnormal
TEST(ProductionServiceTest, CalculateActualQty_YieldZero_ShouldThrow)
TEST(ProductionServiceTest, ProcessJob_EmptyQueue_ShouldDoNothing)
TEST(ProductionServiceTest, TimeProvider_SimulateElapsed_ShouldFinish)
```

---

## 연관 컴포넌트

| 레이어 | 클래스 |
|--------|--------|
| Service | `ProductionService` |
| Controller | `ProductionController` |
| View | `ProductionView` |
| Repository (인터페이스) | `IProductionRepository`, `IInventoryRepository` |
| Repository (구현체) | `InMemoryProductionRepository`, `ProductionJobJsonRepository` |
| 시간 처리 | `ITimeProvider`, `SystemTimeProvider`, `MockTimeProvider` |
