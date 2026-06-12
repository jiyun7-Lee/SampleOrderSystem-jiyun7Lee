# PRD — FR-10~11. 모니터링

> 상위 문서: [PRD.md](../../PRD.md) | 관련 Phase: [Phase5_모니터링_출고처리.md](../../temp/Phase5_모니터링_출고처리.md)

---

## FR-10 주문 모니터링

### 기능

- 상태별 주문 수 표시

### 표시 대상 상태

| 상태 | 표시 여부 |
|------|----------|
| `RESERVED` | ✅ |
| `PRODUCING` | ✅ |
| `CONFIRMED` | ✅ |
| `RELEASE` | ✅ |
| `REJECTED` | ❌ 제외 |

### 출력 형식 (예시)

```
[주문 현황]
RESERVED  : 3건
PRODUCING : 1건
CONFIRMED : 5건
RELEASE   : 12건
```

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(MonitoringServiceTest, GetOrderStats_ShouldCountByStatus)
TEST(MonitoringServiceTest, GetOrderStats_ShouldExcludeRejected)

// Abnormal
TEST(MonitoringServiceTest, GetOrderStats_NoOrders_ShouldReturnZeros)
```

---

## FR-11 재고 모니터링

### 기능

- 시료별 현재 재고 + 상태 표기

### 재고 상태 판단 기준

| 상태 | 조건 |
|------|------|
| `여유` | `availableStock > 0` (주문 대비 재고 충분) |
| `부족` | `availableStock <= 0` but `currentStock > 0` |
| `고갈` | `currentStock == 0` |

> `availableStock = currentStock - reservedStock`

### 출력 항목

| 컬럼 | 설명 |
|------|------|
| Sample ID | 시료 ID |
| Name | 시료명 |
| currentStock | 현재 재고 수량 |
| reservedStock | 확보된 수량 |
| availableStock | 주문 가능 수량 |
| 상태 | 여유 / 부족 / 고갈 |

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(MonitoringServiceTest, GetInventoryStatus_Sufficient_ShouldShowYeoyu)
TEST(MonitoringServiceTest, GetInventoryStatus_Insufficient_ShouldShowBujok)
TEST(MonitoringServiceTest, GetInventoryStatus_ZeroStock_ShouldShowGogal)

// Abnormal
TEST(MonitoringServiceTest, GetInventoryStatus_NoSamples_ShouldReturnEmptyList)
```

---

## 연관 컴포넌트

| 레이어 | 클래스 |
|--------|--------|
| Service | `MonitoringService` |
| Controller | `MonitoringController` |
| View | `MonitoringView` |
| Repository (인터페이스) | `IOrderRepository`, `IInventoryRepository`, `ISampleRepository` |
