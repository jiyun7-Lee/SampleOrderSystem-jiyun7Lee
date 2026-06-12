# PRD — FR-04~06. 주문 관리

> 상위 문서: [PRD.md](../../PRD.md) | 관련 Phase: [Phase3_주문관리.md](../../temp/Phase3_주문관리.md)

---

## FR-04 주문 접수

### 입력

| 필드 | 타입 | 제약 |
|------|------|------|
| Sample ID | String | 등록된 시료여야 함 |
| Customer Name | String | 공백 불가 |
| Quantity | int | 1 이상 |

### 결과

- 상태 `RESERVED`로 주문 생성
- 주문 번호 자동 생성: `ORD-YYYYMMDD-NNNN`

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(OrderServiceTest, CreateOrder_ValidInput_ShouldCreateReserved)

// Abnormal
TEST(OrderServiceTest, CreateOrder_UnknownSampleId_ShouldThrow)
TEST(OrderServiceTest, CreateOrder_EmptyCustomerName_ShouldThrow)
TEST(OrderServiceTest, CreateOrder_ZeroQuantity_ShouldThrow)
TEST(OrderServiceTest, CreateOrder_NegativeQuantity_ShouldThrow)
```

---

## FR-05 주문 승인

### 승인 흐름

```
availableStock >= orderQuantity
  → RESERVED → CONFIRMED
  → reservedStock += orderQuantity

availableStock < orderQuantity
  → RESERVED → PRODUCING
  → ProductionJob 생성
    requiredQuantity = orderQuantity - availableStock
```

### 재고 정책

| 용어 | 필드명 | 계산 |
|------|--------|------|
| 현재 재고 | `currentStock` | 실제 생산 완료된 정상 시료 수량 |
| 필요 재고 | `reservedStock` | 이미 주문이 확보한 수량 |
| 주문 가능 재고 | `availableStock` | `currentStock - reservedStock` |

> 승인 여부는 반드시 **availableStock** 기준으로 판단한다.

### 주문 수량 보호 규칙

- 주문이 확보한 수량(`reservedStock`)은 다른 주문이 사용할 수 없다.
- 신규 주문의 `availableStock` 계산 시 기존 `reservedStock`은 제외된다.

### 부족 수량 계산 규칙

- 부족 수량은 **주문 승인 시점에 1회만** 계산하여 `ProductionJob`에 저장한다.
- 생산 시작 후 부족 수량을 재계산하지 않는다.

```
requiredQuantity = orderQuantity - availableStock
```

### 생산 여부 결정 규칙

- 생산 여부는 **주문 승인 시점에 단 한 번만** 판단한다.
- 생산 작업이 생성된 이후에는 재고가 변경되더라도 재판단하지 않는다.
- 이미 생성된 생산 작업은 취소되지 않는다.

### 관련 테스트 케이스 (gmock)

```cpp
// Normal — 재고 충분
TEST(OrderServiceTest, ApproveOrder_SufficientStock_ShouldConfirm)
TEST(OrderServiceTest, ApproveOrder_StockExactlyEqual_ShouldConfirm)

// Normal — 재고 부족
TEST(OrderServiceTest, ApproveOrder_InsufficientStock_ShouldProducing)
TEST(OrderServiceTest, ApproveOrder_ZeroStock_ShouldProducing)

// Abnormal
TEST(OrderServiceTest, ApproveOrder_AlreadyConfirmed_ShouldThrow)
TEST(OrderServiceTest, ApproveOrder_RejectedOrder_ShouldThrow)
TEST(OrderServiceTest, ApproveOrder_UnknownOrderId_ShouldThrow)
TEST(OrderServiceTest, ApproveOrder_StockOneLess_ShouldProducing)
```

---

## FR-06 주문 거절

### 기능

- `RESERVED` 상태 주문 → 즉시 `REJECTED`

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(OrderServiceTest, RejectOrder_Reserved_ShouldReject)

// Abnormal
TEST(OrderServiceTest, RejectOrder_AlreadyRejected_ShouldThrow)
TEST(OrderServiceTest, RejectOrder_Confirmed_ShouldThrow)
TEST(OrderServiceTest, RejectOrder_UnknownOrderId_ShouldThrow)
```

---

## 연관 컴포넌트

| 레이어 | 클래스 |
|--------|--------|
| Service | `OrderService`, `InventoryService` |
| Controller | `OrderController` |
| View | `OrderView` |
| Repository (인터페이스) | `IOrderRepository`, `IInventoryRepository`, `IProductionRepository` |
| Repository (구현체) | `InMemoryOrderRepository`, `OrderJsonRepository` 등 |
