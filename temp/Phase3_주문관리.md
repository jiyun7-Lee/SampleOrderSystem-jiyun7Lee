# Phase 3 — 주문 관리 기능 (FR-04~06)

## 목표

주문 접수(RESERVED), 승인(CONFIRMED/PRODUCING), 거절(REJECTED) 기능을 구현한다.  
재고 정책(`availableStock = currentStock - reservedStock`)에 따른 자동 분기 처리가 핵심이다.

## 담당 에이전트

| 단계 | 에이전트 |
|------|---------|
| 테스트 작성 | `unit-test` |
| 구현 | `feature` |
| SOLID 검토 | `clean-code` |
| 빌드·실행 | `tester` |
| 커밋 | `git-manager` |

---

## Step 3-1: InventoryService 구현

### 기능

| 메서드 | 설명 |
|--------|------|
| `GetInventory(sampleId)` | 시료별 재고 조회 |
| `GetAvailableStock(sampleId)` | 주문 가능 재고 반환 |
| `AllocateStock(sampleId, qty)` | reservedStock 증가 |
| `AddStock(sampleId, qty)` | currentStock 증가 (생산 완료 시) |
| `DeductStock(sampleId, qty)` | currentStock 차감 (출고 시) |

### 재고 정책

```
availableStock = currentStock - reservedStock
```

- 주문이 확보한 수량(`reservedStock`)은 다른 주문이 사용 불가
- 주문 승인 여부는 반드시 `availableStock` 기준으로 판단

### 단위 테스트 케이스 (gmock)

```cpp
// tests/service/InventoryServiceTest.cpp
TEST(InventoryServiceTest, GetAvailableStock_ReturnsCurrentMinusReserved)
TEST(InventoryServiceTest, AllocateStock_IncreasesReservedStock)
TEST(InventoryServiceTest, AllocateStock_ExceedsAvailable_ShouldThrow)
TEST(InventoryServiceTest, AddStock_IncreasesCurrentStock)
TEST(InventoryServiceTest, DeductStock_DecreasesCurrentStock)
```

### 완료 기준

- [ ] InventoryService 구현 완료
- [ ] 재고 정책 로직 정확
- [ ] 단위 테스트 PASS

---

## Step 3-2: OrderService 구현

### FR-04 주문 접수

```cpp
Order PlaceOrder(sampleId, customerName, quantity);
// 상태: RESERVED
// orderId 자동 생성: ORD-YYYYMMDD-NNNN
```

### FR-05 주문 승인

```cpp
void ApproveOrder(orderId);
```

**처리 흐름**

```
availableStock >= orderQuantity
  → Order.status = CONFIRMED
  → InventoryService.AllocateStock(+orderQuantity)

availableStock < orderQuantity
  → Order.status = PRODUCING
  → requiredQuantity = orderQuantity - availableStock
  → ProductionJob 생성 및 ProductionQueue에 추가
```

> 생산 여부는 승인 시점에 **단 한 번만** 판단 — 이후 재고 변경 무시

### FR-06 주문 거절

```cpp
void RejectOrder(orderId);
// Order.status = REJECTED
```

### 단위 테스트 케이스 (gmock)

```cpp
// tests/service/OrderServiceTest.cpp
class MockOrderRepository : public IOrderRepository { /* MOCK_METHOD */ };
class MockInventoryRepository : public IInventoryRepository { /* MOCK_METHOD */ };
class MockProductionRepository : public IProductionRepository { /* MOCK_METHOD */ };
class MockTimeProvider : public ITimeProvider { /* MOCK_METHOD */ };

TEST(OrderServiceTest, PlaceOrder_ValidInput_ShouldBeReserved)
TEST(OrderServiceTest, PlaceOrder_InvalidSampleId_ShouldThrow)
TEST(OrderServiceTest, ApproveOrder_SufficientStock_ShouldConfirm)
TEST(OrderServiceTest, ApproveOrder_SufficientStock_ShouldAllocateStock)
TEST(OrderServiceTest, ApproveOrder_InsufficientStock_ShouldProducing)
TEST(OrderServiceTest, ApproveOrder_InsufficientStock_ShouldCreateProductionJob)
TEST(OrderServiceTest, ApproveOrder_NotReserved_ShouldThrow)
TEST(OrderServiceTest, RejectOrder_ShouldBeRejected)
TEST(OrderServiceTest, RejectOrder_NotReserved_ShouldThrow)
```

### 완료 기준

- [ ] OrderService 구현 완료
- [ ] 승인 시 재고 분기 로직 정확
- [ ] ProductionJob 자동 생성 동작
- [ ] 단위 테스트 PASS

---

## Step 3-3: OrderController 구현

```cpp
class OrderController {
public:
    void HandlePlaceOrder();    // [2] 시료 주문
    void HandleApproveReject(); // [3] 주문 승인/거절
};
```

### 완료 기준

- [ ] 주문 접수 흐름 동작
- [ ] RESERVED 목록 → 번호 선택 → 승인/거절 처리

---

## Step 3-4: OrderView 구현

### [2] 시료 주문 UI

```
[2] 시료 주문
시료 ID   > S-001
고객명    > 삼성전자 파운드리
주문 수량 > 200

[입력 확인]
시료   SiC 파워기판-6인치 (S-001)
고객   삼성전자 파운드리
수량   200 ea

[Y] 예약 접수   [N] 취소
선택 > _
```

### [3] 주문 승인/거절 UI

```
[3] 주문 승인/거절
승인 대기 중인 예약 목록 (RESERVED)

번호   주문번호      고객           시료              수량    상태
[1]   ORD-0041   LG이노텍   산화막 웨이퍼-SiO2   300 ea  RESERVED
[2]   ORD-0042   SK하이닉스  실리콘 웨이퍼-8인치  150 ea  RESERVED

승인할 번호 > _
```

### 완료 기준

- [ ] 주문 접수 UI 완성
- [ ] 승인/거절 UI 완성
- [ ] 상태 변경 결과 표시

---

## Step 3-5: 메인 메뉴 [2], [3] 연결

### 완료 기준

- [ ] 메인 메뉴 → [2] → 주문 접수
- [ ] 메인 메뉴 → [3] → 주문 승인/거절

---

## Phase 3 완료 기준

- [ ] FR-04 주문 접수 동작 확인
- [ ] FR-05 주문 승인 (재고 충분 → CONFIRMED, 부족 → PRODUCING) 동작 확인
- [ ] FR-06 주문 거절 동작 확인
- [ ] 단위 테스트 PASS
- [ ] 커버리지 측정 완료
- [ ] `[feat] Phase 3 — 주문 관리 기능` 커밋 완료
