# PRD — FR-09. 출고 처리

> 상위 문서: [PRD.md](../../PRD.md) | 관련 Phase: [Phase5_모니터링_출고처리.md](../../temp/Phase5_모니터링_출고처리.md)

---

## FR-09 출고 처리

### 기능

- `CONFIRMED` 상태 주문 목록 표시
- 담당자가 주문을 선택하여 출고 실행

### 출고 처리 흐름

```
CONFIRMED 주문 선택
    ↓
재고 차감 (currentStock -= quantity)
    ↓
CONFIRMED → RELEASE
```

### 재고 차감 규칙

- 출고 시 `currentStock`에서 주문 수량만큼 차감
- 출고 완료 시 해당 주문의 `reservedStock`도 해제

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(ReleaseServiceTest, ReleaseOrder_Confirmed_ShouldRelease)
TEST(ReleaseServiceTest, ReleaseOrder_ShouldDeductStock)

// Abnormal
TEST(ReleaseServiceTest, ReleaseOrder_NotConfirmed_ShouldThrow)
TEST(ReleaseServiceTest, ReleaseOrder_AlreadyReleased_ShouldThrow)
TEST(ReleaseServiceTest, ReleaseOrder_UnknownOrderId_ShouldThrow)
TEST(ReleaseServiceTest, GetConfirmedOrders_Empty_ShouldReturnEmptyList)
```

---

## 연관 컴포넌트

| 레이어 | 클래스 |
|--------|--------|
| Service | `ReleaseService` |
| Controller | `ReleaseController` |
| View | `ReleaseView` |
| Repository (인터페이스) | `IOrderRepository`, `IInventoryRepository` |
| Repository (구현체) | `InMemoryOrderRepository`, `OrderJsonRepository` 등 |
