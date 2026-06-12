# Phase 4 — 생산라인 관리 (FR-07~08)

## 목표

FIFO 생산 큐 및 실시간 생산 처리 로직을 구현한다.  
생산 완료 감지(`ITimeProvider` 기반), 초과 생산분 즉시 반영, `PRODUCING → CONFIRMED` 상태 전이가 핵심이다.

## 담당 에이전트

| 단계 | 에이전트 |
|------|---------|
| 테스트 작성 | `unit-test` |
| 구현 | `feature` |
| SOLID 검토 | `clean-code` |
| 빌드·실행 | `tester` |
| 커밋 | `git-manager` |

---

## Step 4-1: ProductionQueue 구현 (FIFO)

### 구조

```cpp
// src/model/domain/ProductionQueue.h
class ProductionQueue {
    std::queue<ProductionJob> queue_;
public:
    void Enqueue(const ProductionJob& job);
    std::optional<ProductionJob> Peek() const;  // 현재 처리 중인 작업
    void Dequeue();                              // 완료된 작업 제거
    std::vector<ProductionJob> GetWaitingJobs() const; // 대기 목록
    bool IsEmpty() const;
};
```

### 규칙

- 생산라인은 **1개만** 존재
- 동시에 **하나의 작업만** 처리
- **FIFO** 방식 순차 처리

### 단위 테스트 케이스

```cpp
TEST(ProductionQueueTest, Enqueue_ShouldAddToQueue)
TEST(ProductionQueueTest, Peek_ReturnsFirstJob)
TEST(ProductionQueueTest, Dequeue_RemovesFirstJob)
TEST(ProductionQueueTest, FIFO_Order_ShouldBePreserved)
TEST(ProductionQueueTest, IsEmpty_WhenNoJobs_ShouldBeTrue)
```

### 완료 기준

- [ ] ProductionQueue FIFO 동작 확인
- [ ] 단위 테스트 PASS

---

## Step 4-2: ProductionService 구현 (실시간 생산 처리)

### 핵심 로직

```cpp
class ProductionService {
public:
    ProductionService(
        IProductionRepository& prodRepo,
        IInventoryRepository& invRepo,
        IOrderRepository& orderRepo,
        ITimeProvider& timeProvider
    );

    void CheckAndCompleteProduction(); // 현재 작업 완료 여부 확인
    ProductionStatus GetCurrentStatus() const;
    std::vector<ProductionJob> GetWaitingQueue() const;
};
```

### 생산 완료 감지

```cpp
bool IsCompleted(const ProductionJob& job) {
    auto now = timeProvider_.Now();
    return now >= job.expectedFinishTime;
}
```

### 생산 완료 처리 흐름

```
1. timeProvider.Now() >= job.expectedFinishTime
2. InventoryService.AddStock(sampleId, actualProductionQuantity)
3. Order.status = PRODUCING → CONFIRMED
4. ProductionQueue.Dequeue()
5. 다음 대기 작업이 있으면 자동 시작
```

### 단위 테스트 케이스 (gmock — ITimeProvider 주입)

```cpp
class MockTimeProvider : public ITimeProvider {
public:
    MOCK_METHOD(std::chrono::system_clock::time_point, Now, (), (const, override));
};

// 시간 조작으로 생산 완료 시뮬레이션
TEST(ProductionServiceTest, CheckCompletion_BeforeFinishTime_ShouldNotComplete)
TEST(ProductionServiceTest, CheckCompletion_AfterFinishTime_ShouldComplete)
TEST(ProductionServiceTest, OnComplete_ShouldUpdateOrderToConfirmed)
TEST(ProductionServiceTest, OnComplete_ShouldAddStockToInventory)
TEST(ProductionServiceTest, OnComplete_ShouldDequeueJob)
TEST(ProductionServiceTest, MultipleJobs_ShouldProcessInFIFOOrder)
```

> 실제 시간을 기다리는 테스트는 허용하지 않는다.  
> `MockTimeProvider`로 `expectedFinishTime` 이후 시각을 반환하여 완료를 시뮬레이션한다.

### 완료 기준

- [ ] ProductionService 구현 완료
- [ ] 실시간 생산 완료 감지 동작
- [ ] 단위 테스트 PASS (시간 시뮬레이션 포함)

---

## Step 4-3: 생산량·생산시간 계산

### 생산량 계산

```cpp
int CalcActualProductionQty(int requiredQty, double yieldRate) {
    return static_cast<int>(std::ceil(
        static_cast<double>(requiredQty) / (yieldRate * 0.9)
    ));
}
```

### 생산시간 계산

```cpp
double CalcTotalProductionTime(double avgTime, int actualQty) {
    return avgTime * actualQty; // 초
}
```

### 예시 검증

```
requiredQty = 100, yieldRate = 0.1
actualQty   = ceil(100 / (0.1 * 0.9)) = ceil(1111.11) = 1112

avgTime = 3초, actualQty = 100
totalTime = 300초
```

### 단위 테스트 케이스

```cpp
TEST(ProductionCalcTest, ActualQty_WithHighYield_ShouldBeSmaller)
TEST(ProductionCalcTest, ActualQty_WithLowYield_ShouldBeLarger)
TEST(ProductionCalcTest, ActualQty_Example_100_01_ShouldBe1112)
TEST(ProductionCalcTest, TotalTime_3sec_100ea_ShouldBe300)
```

### 완료 기준

- [ ] 계산 공식 정확
- [ ] 단위 테스트 PASS

---

## Step 4-4: 초과 생산분 즉시 가용 재고 반영

### 규칙

생산 진행 중 `actualProductionQuantity` 개 생산이 완료되면,  
주문이 확보한 수량(`requiredQuantity`) 외의 **초과분은 즉시 `currentStock`에 반영**된다.

```
예시:
  주문1 수량(requiredQuantity)  = 100
  실생산량(actualQty)           = 120
  현재 생산 완료                = 115

  주문1 확보(reservedStock)    = 100
  즉시 가용(availableStock)    += 15
```

### 완료 기준

- [ ] 생산 완료 시 초과분 `currentStock` 반영 동작 확인
- [ ] 초과분이 신규 주문 승인에 즉시 사용 가능한지 확인

---

## Step 4-5: ProductionController 및 ProductionView 구현

### ProductionView 출력 예시

```
[5] 생산라인 조회   FIFO 방식

생산라인 1개 (단일 라인)   현재 상태: RUNNING

[ 현재 처리 중 ]
주문번호  ORD-20260416-0038   시료  SiC 파워기판-6인치
주문량    80 ea   재고 30 ea → 부족 50 ea → 실생산량 61 ea (수율 0.92)
진행      ████████░░░░  72%   완료 예정  09:49

[ 대기 중인 주문 (FIFO 순) ]
순서  주문번호    시료                  주문량   부족분   실생산량   예상완료
 1   ORD-0040  산화막 웨이퍼-SiO2    150 ea  150 ea  190 ea   11:43
 2   ORD-0043  SiC 파워기판-6인치   200 ea  170 ea  206 ea   14:28
```

### 완료 기준

- [ ] 현재 생산 중인 작업 정보 표시
- [ ] 대기 큐 목록 표시 (FIFO 순)
- [ ] 진행률 및 완료 예정 시각 표시

---

## Step 4-6: 메인 메뉴 [5] 생산라인 조회 연결

### 완료 기준

- [ ] 메인 메뉴 → [5] → ProductionController 진입
- [ ] 메인 메뉴 현황판 "생산라인 N건 대기" 정상 표시

---

## Phase 4 완료 기준

- [ ] FR-07 생산 큐 FIFO 관리 동작 확인
- [ ] FR-08 생산 처리 (실시간 완료 감지, 상태 전이) 동작 확인
- [ ] 생산량·생산시간 계산 정확
- [ ] 초과 생산분 즉시 가용 재고 반영 확인
- [ ] 단위 테스트 PASS (MockTimeProvider 사용)
- [ ] 커버리지 측정 완료
- [ ] `[feat] Phase 4 — 생산라인 관리` 커밋 완료
