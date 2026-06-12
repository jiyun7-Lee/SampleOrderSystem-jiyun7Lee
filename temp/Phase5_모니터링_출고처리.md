# Phase 5 — 모니터링 및 출고 처리 (FR-09~11)

## 목표

출고 처리(`CONFIRMED → RELEASE`)와 상태별 주문·재고 모니터링 기능을 구현한다.  
메인 메뉴 **[4] 모니터링**, **[6] 출고 처리**가 완전히 동작하는 상태를 목표로 한다.

## 담당 에이전트

| 단계 | 에이전트 |
|------|---------|
| 테스트 작성 | `unit-test` |
| 구현 | `feature` |
| SOLID 검토 | `clean-code` |
| 빌드·실행 | `tester` |
| 커밋 | `git-manager` |

---

## Step 5-1: ReleaseService 구현 (FR-09)

### 기능

```cpp
class ReleaseService {
public:
    std::vector<Order> GetConfirmedOrders() const;
    void ReleaseOrder(const std::string& orderId);
};
```

### 처리 흐름

```
1. CONFIRMED 상태 주문 목록 조회
2. 선택한 주문 출고 실행
3. InventoryService.DeductStock(sampleId, orderQuantity)  // 재고 차감
4. InventoryService.DeallocateStock(sampleId, orderQuantity) // reservedStock 감소
5. Order.status = CONFIRMED → RELEASE
```

### 단위 테스트 케이스 (gmock)

```cpp
TEST(ReleaseServiceTest, GetConfirmedOrders_ShouldReturnOnlyConfirmed)
TEST(ReleaseServiceTest, ReleaseOrder_ShouldChangeStatusToRelease)
TEST(ReleaseServiceTest, ReleaseOrder_ShouldDeductStock)
TEST(ReleaseServiceTest, ReleaseOrder_ShouldDeallocateReservedStock)
TEST(ReleaseServiceTest, ReleaseOrder_NotConfirmed_ShouldThrow)
TEST(ReleaseServiceTest, ReleaseOrder_NotFound_ShouldThrow)
```

### 완료 기준

- [ ] ReleaseService 구현 완료
- [ ] 재고 차감 및 상태 전이 정확
- [ ] 단위 테스트 PASS

---

## Step 5-2: MonitoringService 구현 (FR-10~11)

### FR-10 주문량 확인

```cpp
struct OrderStatusSummary {
    int reservedCount;
    int producingCount;
    int confirmedCount;
    int releaseCount;
    // REJECTED 제외
};

OrderStatusSummary GetOrderSummary() const;
```

### FR-11 재고량 확인

```cpp
enum class StockStatus { SUFFICIENT, SHORTAGE, DEPLETED };

struct InventoryStatus {
    std::string sampleId;
    std::string sampleName;
    int currentStock;
    StockStatus status;
    double utilizationRate; // currentStock / (currentStock + 주문대기) 비율
};

std::vector<InventoryStatus> GetInventoryStatus() const;
```

### 재고 상태 판단 기준

| 상태 | 조건 |
|------|------|
| `여유` (SUFFICIENT) | `availableStock > 0` |
| `부족` (SHORTAGE) | `availableStock <= 0` (reservedStock > 0) |
| `고갈` (DEPLETED) | `currentStock == 0` |

### 단위 테스트 케이스 (gmock)

```cpp
TEST(MonitoringServiceTest, GetOrderSummary_ShouldExcludeRejected)
TEST(MonitoringServiceTest, GetOrderSummary_CountsCorrectByStatus)
TEST(MonitoringServiceTest, GetInventoryStatus_ZeroStock_ShouldBeDepleted)
TEST(MonitoringServiceTest, GetInventoryStatus_InsufficientAvailable_ShouldBeShortage)
TEST(MonitoringServiceTest, GetInventoryStatus_SufficientAvailable_ShouldBeSufficient)
```

### 완료 기준

- [ ] MonitoringService 구현 완료
- [ ] 상태별 주문 수 집계 정확 (REJECTED 제외)
- [ ] 재고 상태(여유/부족/고갈) 판단 정확
- [ ] 단위 테스트 PASS

---

## Step 5-3: ReleaseController / MonitoringController 구현

### ReleaseController

```cpp
class ReleaseController {
public:
    void HandleRelease(); // CONFIRMED 목록 표시 → 선택 → 출고 처리
};
```

### MonitoringController

```cpp
class MonitoringController {
public:
    void HandleMonitoring(); // [1] 주문량 확인 / [2] 재고량 확인
};
```

### 완료 기준

- [ ] ReleaseController 구현 완료
- [ ] MonitoringController 구현 완료

---

## Step 5-4: ReleaseView / MonitoringView 구현

### [6] 출고 처리 UI

```
[6] 출고 처리

출고 가능 주문 (CONFIRMED)
번호   주문번호          고객       시료                 수량
[1]   ORD-20260416-0042  SK하이닉스  실리콘 웨이퍼-8인치  150 ea
[2]   ORD-20260416-0035  DB하이텍   포토레지스트-PR7     400 ea

출고할 번호 > 1

출고 처리 완료.
주문번호  ORD-20260416-0042
출고수량  150 ea
처리일시  2026-06-12 09:34:02
상태      CONFIRMED → RELEASE
```

### [4] 모니터링 UI

```
[4] 모니터링   2026-06-12 09:32:15

[1] 주문량 확인   [2] 재고량 확인   [0] 뒤로

--- 상태별 주문 현황 ---
RESERVED    3건
CONFIRMED   8건
PRODUCING   3건  ← 생산라인 대기
RELEASE    18건

--- 재고 현황 ---
시료명                  재고      상태   잔여율
실리콘 웨이퍼-8인치    480 ea   여유    80%  ████████░░
GaN 에피택셀-4인치     220 ea   여유    44%  ████░░░░░░
SiC 파워기판-6인치      30 ea   부족     6%  █░░░░░░░░░
산화막 웨이퍼-SiO2       0 ea   고갈     0%  ░░░░░░░░░░
```

### 완료 기준

- [ ] 출고 처리 UI 완성
- [ ] 모니터링 UI (주문량·재고량) 완성
- [ ] 상태 컬러 표기 (텍스트로 대체 가능)

---

## Step 5-5: 메인 메뉴 [4], [6] 연결

### 완료 기준

- [ ] 메인 메뉴 → [4] → MonitoringController 진입
- [ ] 메인 메뉴 → [6] → ReleaseController 진입

---

## Phase 5 완료 기준

- [ ] FR-09 출고 처리 (`CONFIRMED → RELEASE`) 동작 확인
- [ ] FR-10 주문 모니터링 (상태별 수량, REJECTED 제외) 동작 확인
- [ ] FR-11 재고 모니터링 (여유/부족/고갈) 동작 확인
- [ ] 단위 테스트 PASS
- [ ] 커버리지 측정 완료
- [ ] `[feat] Phase 5 — 모니터링 및 출고 처리` 커밋 완료
