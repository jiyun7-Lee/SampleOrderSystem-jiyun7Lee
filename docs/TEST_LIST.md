# Test List

> 상태 기호: ⬜ 미실행 | 🔴 FAIL | ✅ PASS

---

## Phase 1 — 도메인 모델 및 InMemory Repository

**파일**: `tests/domain/DomainModelTest.cpp`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 1 | Sample_ValidConstruction_ShouldSetFieldsCorrectly | SampleTest | 유효한 필드 설정 | 모든 필드 값 정확히 일치 | Normal | ✅ PASS | 2026-06-12 |
| 2 | Sample_YieldRate_Zero_ShouldBeInvalid | SampleTest | yieldRate = 0.0 | yieldRate <= 0.0 | Abnormal | ✅ PASS | 2026-06-12 |
| 3 | Sample_YieldRate_One_ShouldBeValid | SampleTest | yieldRate = 1.0 | 0.0 < yieldRate <= 1.0 | Normal | ✅ PASS | 2026-06-12 |
| 4 | Sample_YieldRate_OverOne_ShouldBeInvalid | SampleTest | yieldRate = 1.1 | yieldRate > 1.0 | Abnormal | ✅ PASS | 2026-06-12 |
| 5 | Order_ValidConstruction_ShouldSetFieldsCorrectly | OrderTest | 유효한 필드 설정 | 모든 필드 값 정확히 일치 | Normal | ✅ PASS | 2026-06-12 |
| 6 | Order_DefaultStatus_ShouldBeReserved | OrderTest | 초기 상태 | status == RESERVED | Normal | ✅ PASS | 2026-06-12 |
| 7 | Order_Quantity_ShouldBePositive | OrderTest | 수량 = 5 | quantity > 0 | Normal | ✅ PASS | 2026-06-12 |
| 8 | Inventory_AvailableStock_ReturnsCorrectValue | InventoryTest | current=100, reserved=30 | availableStock() == 70 | Normal | ✅ PASS | 2026-06-12 |
| 9 | Inventory_AvailableStock_WhenReservedExceedsCurrent | InventoryTest | current=10, reserved=30 | availableStock() == -20 | Abnormal | ✅ PASS | 2026-06-12 |
| 10 | Inventory_ZeroStock_AvailableIsZero | InventoryTest | current=0, reserved=0 | availableStock() == 0 | Normal | ✅ PASS | 2026-06-12 |
| 11 | ProductionJob_ValidConstruction_ShouldSetFieldsCorrectly | ProductionJobTest | 유효한 필드 설정 | 모든 필드 값 정확히 일치 | Normal | ✅ PASS | 2026-06-12 |
| 12 | ProductionJob_RequiredQuantity_ShouldBePositive | ProductionJobTest | requiredQuantity = 10 | requiredQuantity > 0 | Normal | ✅ PASS | 2026-06-12 |
| 13 | Save_NewSample_ShouldBeFoundById | InMemorySampleRepositoryTest::Save | 신규 Sample 저장 | FindById 성공, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 14 | Save_ExistingSample_ShouldUpdate | InMemorySampleRepositoryTest::Save | 동일 ID로 재저장 | 이름이 업데이트된 값으로 반환 | Normal | ✅ PASS | 2026-06-12 |
| 15 | FindById_NonExistent_ShouldReturnNullopt | InMemorySampleRepositoryTest::FindById | 존재하지 않는 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 16 | FindAll_MultipleItems_ShouldReturnAll | InMemorySampleRepositoryTest::FindAll | 3건 저장 | size == 3 | Normal | ✅ PASS | 2026-06-12 |
| 17 | ExistsById_AfterSave_ShouldReturnTrue | InMemorySampleRepositoryTest::ExistsById | 저장 후 조회 | true 반환 | Normal | ✅ PASS | 2026-06-12 |
| 18 | ExistsById_BeforeSave_ShouldReturnFalse | InMemorySampleRepositoryTest::ExistsById | 저장 전 조회 | false 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 19 | FindByName_ShouldReturnMatchingItems | InMemorySampleRepositoryTest::FindByName | 동일 이름 2건 저장 | size == 2, 모든 name 일치 | Normal | ✅ PASS | 2026-06-12 |
| 20 | DeleteById_AfterSave_ShouldRemoveItem | InMemorySampleRepositoryTest::DeleteById | 저장 후 삭제 | FindById nullopt 반환 | Normal | ✅ PASS | 2026-06-12 |
| 21 | Save_NewOrder_ShouldBeFoundById | InMemoryOrderRepositoryTest::Save | 신규 Order 저장 | FindById 성공, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 22 | FindByStatus_ShouldReturnMatchingOrders | InMemoryOrderRepositoryTest::FindByStatus | RESERVED 2건, CONFIRMED 1건 저장 | RESERVED size == 2 | Normal | ✅ PASS | 2026-06-12 |
| 23 | DeleteById_ShouldRemoveOrder | InMemoryOrderRepositoryTest::DeleteById | 저장 후 삭제 | ExistsById false, FindById nullopt | Normal | ✅ PASS | 2026-06-12 |
| 24 | Save_NewInventory_ShouldBeFoundBySampleId | InMemoryInventoryRepositoryTest::Save | 신규 Inventory 저장 | FindBySampleId 성공, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 25 | ExistsBySampleId_AfterSave_ShouldReturnTrue | InMemoryInventoryRepositoryTest::ExistsBySampleId | 저장 후 조회 | true 반환 | Normal | ✅ PASS | 2026-06-12 |
| 26 | Save_NewJob_ShouldBeFoundById | InMemoryProductionRepositoryTest::Save | 신규 ProductionJob 저장 | FindById 성공, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 27 | FindByOrderId_ShouldReturnMatchingJobs | InMemoryProductionRepositoryTest::FindByOrderId | ORD-001 관련 Job 2건 저장 | size == 2, 모든 orderId 일치 | Normal | ✅ PASS | 2026-06-12 |

**파일**: `tests/domain/TimeProviderTest.cpp`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 28 | Now_ShouldReturnCurrentTime | SystemTimeProviderTest::Now | Now() 호출 | system_clock::now()와 1초 이내 차이 | Normal | ✅ PASS | 2026-06-12 |

---

## Phase 2 — SampleService (서비스 레이어)

**파일**: `tests/service/SampleServiceTest.cpp`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 29 | RegisterSample_ValidSample_ShouldCallSave | SampleService::RegisterSample | 유효한 Sample, ID 미존재 | Save 1회 호출 | Normal | ✅ PASS | 2026-06-12 |
| 30 | GetAllSamples_ShouldReturnAllFromRepository | SampleService::GetAllSamples | 2건 저장된 상태 | 반환 size == 2 | Normal | ✅ PASS | 2026-06-12 |
| 31 | FindSampleById_ExistingId_ShouldReturnSample | SampleService::FindSampleById | 존재하는 ID | optional에 값 존재, sampleId 일치 | Normal | ✅ PASS | 2026-06-12 |
| 32 | SearchSamplesByName_ShouldReturnMatchingResults | SampleService::SearchByName | "Alpha" 매칭 1건 | 반환 size == 1 | Normal | ✅ PASS | 2026-06-12 |
| 33 | RegisterSample_DuplicateId_ShouldThrow | SampleService::RegisterSample | 중복 ID | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 34 | RegisterSample_EmptyId_ShouldThrow | SampleService::RegisterSample | 빈 ID | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 35 | RegisterSample_NegativeYield_ShouldThrow | SampleService::RegisterSample | yieldRate < 0 | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 36 | RegisterSample_YieldOverOne_ShouldThrow | SampleService::RegisterSample | yieldRate > 1.0 | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 37 | RegisterSample_NegativeAvgProductionTime_ShouldThrow | SampleService::RegisterSample | averageProductionTime < 0 | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 38 | FindSampleById_NonExistingId_ShouldReturnNullopt | SampleService::FindSampleById | 존재하지 않는 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |

---

## Phase 3 — 주문 관리 서비스

**파일**: `tests/service/InventoryServiceTest.cpp`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 39 | GetAvailableStock_ReturnsCurrentMinusReserved | InventoryService::GetAvailableStock | currentStock - reservedStock 조회 | 가용 재고(currentStock - reservedStock) 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 40 | AllocateStock_IncreasesReservedStock | InventoryService::AllocateStock | 가용 재고 내 수량 예약 | reservedStock 증가 | Normal | ⬜ 미실행 | 2026-06-12 |
| 41 | AllocateStock_ExceedsAvailable_ShouldThrow | InventoryService::AllocateStock | 예약 수량 > 가용 재고 | 예외 throw | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 42 | AddStock_IncreasesCurrentStock | InventoryService::AddStock | 재고 추가 | currentStock 증가 | Normal | ⬜ 미실행 | 2026-06-12 |
| 43 | DeductStock_DecreasesCurrentStock | InventoryService::DeductStock | 재고 차감 | currentStock 감소 | Normal | ⬜ 미실행 | 2026-06-12 |

**파일**: `tests/service/OrderServiceTest.cpp`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 44 | PlaceOrder_ValidInput_ShouldBeReserved | OrderService::PlaceOrder | 유효한 sampleId, 고객명, 수량 | 생성된 주문 status == RESERVED | Normal | ⬜ 미실행 | 2026-06-12 |
| 45 | PlaceOrder_InvalidSampleId_ShouldThrow | OrderService::PlaceOrder | 존재하지 않는 sampleId | 예외 throw | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 46 | ApproveOrder_SufficientStock_ShouldConfirm | OrderService::ApproveOrder | RESERVED 주문, 재고 충분 | 주문 status == CONFIRMED | Normal | ⬜ 미실행 | 2026-06-12 |
| 47 | ApproveOrder_SufficientStock_ShouldAllocateStock | OrderService::ApproveOrder | RESERVED 주문, 재고 충분 | AllocateStock 호출됨 | Normal | ⬜ 미실행 | 2026-06-12 |
| 48 | ApproveOrder_InsufficientStock_ShouldProducing | OrderService::ApproveOrder | RESERVED 주문, 재고 부족 | 주문 status == PRODUCING | Normal | ⬜ 미실행 | 2026-06-12 |
| 49 | ApproveOrder_InsufficientStock_ShouldCreateProductionJob | OrderService::ApproveOrder | RESERVED 주문, 재고 부족 | ProductionJob 생성 및 저장 | Normal | ⬜ 미실행 | 2026-06-12 |
| 50 | ApproveOrder_NotReserved_ShouldThrow | OrderService::ApproveOrder | RESERVED 아닌 상태의 주문 | 예외 throw | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 51 | RejectOrder_ShouldBeRejected | OrderService::RejectOrder | RESERVED 주문 거절 | 주문 status == REJECTED | Normal | ⬜ 미실행 | 2026-06-12 |
| 52 | RejectOrder_NotReserved_ShouldThrow | OrderService::RejectOrder | RESERVED 아닌 상태의 주문 거절 시도 | 예외 throw | Abnormal | ⬜ 미실행 | 2026-06-12 |
