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

---

## Phase 4 — 생산 큐 및 생산 서비스

**파일**: `tests/domain/ProductionQueueTest.cpp`  
**Fixture**: `ProductionQueueFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 53 | Enqueue_ShouldAddToQueue | ProductionQueue::Enqueue | Job 1건 추가 | 큐에 Job이 존재함 | Normal | ⬜ 미실행 | 2026-06-12 |
| 54 | Peek_ReturnsFirstJob | ProductionQueue::Peek | Job 추가 후 Peek | 첫 번째 Job 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 55 | Dequeue_RemovesFirstJob | ProductionQueue::Dequeue | Job 1건 추가 후 Dequeue | 큐가 비어있음 | Normal | ⬜ 미실행 | 2026-06-12 |
| 56 | FIFO_Order_ShouldBePreserved | ProductionQueue | Job 복수 추가 | 추가 순서대로 반환됨 | Normal | ⬜ 미실행 | 2026-06-12 |
| 57 | IsEmpty_WhenNoJobs_ShouldBeTrue | ProductionQueue::IsEmpty | 빈 큐 상태 | IsEmpty() == true | Normal | ⬜ 미실행 | 2026-06-12 |
| 58 | GetWaitingJobs_ShouldReturnAllJobsInFIFOOrder | ProductionQueue::GetWaitingJobs | Job 복수 추가 | FIFO 순서로 전체 Job 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 59 | GetWaitingJobs_AfterDequeue_ShouldReflectRemainingJobs | ProductionQueue::GetWaitingJobs | Dequeue 후 조회 | 남은 Job만 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 60 | Peek_WhenEmpty_ShouldReturnNullopt | ProductionQueue::Peek | 빈 큐에서 Peek | nullopt 반환 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 61 | Dequeue_WhenEmpty_ShouldNotThrow | ProductionQueue::Dequeue | 빈 큐에서 Dequeue | 예외 미발생 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 62 | GetWaitingJobs_WhenEmpty_ShouldReturnEmptyVector | ProductionQueue::GetWaitingJobs | 빈 큐 상태 | 빈 vector 반환 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 63 | Dequeue_AllJobs_ShouldBecomeEmpty | ProductionQueue::Dequeue | 전체 Job Dequeue | IsEmpty() == true | Abnormal | ⬜ 미실행 | 2026-06-12 |

**파일**: `tests/service/ProductionServiceTest.cpp`  
**Fixture**: `ProductionServiceFixture`, `ProductionCalcFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 64 | CheckCompletion_BeforeFinishTime_ShouldNotComplete | ProductionService::CheckCompletion | 완료 시각 이전 | 주문 상태 변경 없음 | Normal | ⬜ 미실행 | 2026-06-12 |
| 65 | CheckCompletion_AfterFinishTime_ShouldComplete | ProductionService::CheckCompletion | 완료 시각 이후 | 생산 완료 처리됨 | Normal | ⬜ 미실행 | 2026-06-12 |
| 66 | OnComplete_ShouldUpdateOrderToConfirmed | ProductionService::OnComplete | 생산 완료 시 | 주문 status == CONFIRMED | Normal | ⬜ 미실행 | 2026-06-12 |
| 67 | OnComplete_ShouldAddStockToInventory | ProductionService::OnComplete | 생산 완료 시 | 재고 증가 확인 | Normal | ⬜ 미실행 | 2026-06-12 |
| 68 | OnComplete_ShouldDequeueJob | ProductionService::OnComplete | 생산 완료 시 | 큐에서 해당 Job 제거됨 | Normal | ⬜ 미실행 | 2026-06-12 |
| 69 | MultipleJobs_ShouldProcessInFIFOOrder | ProductionService | Job 복수 등록 | 등록 순서대로 완료 처리됨 | Normal | ⬜ 미실행 | 2026-06-12 |
| 70 | GetCurrentJob_NoJobs_ShouldReturnNullopt | ProductionService::GetCurrentJob | 빈 큐 상태 | nullopt 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 71 | GetCurrentJob_OneJob_ShouldReturnJob | ProductionService::GetCurrentJob | Job 1건 등록 | 해당 Job 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 72 | GetWaitingJobs_NoJobs_ShouldReturnEmpty | ProductionService::GetWaitingJobs | 빈 큐 상태 | 빈 vector 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 73 | GetWaitingJobs_TwoJobs_ShouldReturnSecondOnly | ProductionService::GetWaitingJobs | Job 2건 등록 | 두 번째 Job만 반환 | Normal | ⬜ 미실행 | 2026-06-12 |
| 74 | ActualQty_WithHighYield_ShouldBeSmaller | ProductionService::ActualQty | 높은 수율 | 낮은 수율 대비 실 생산량 적음 | Normal | ⬜ 미실행 | 2026-06-12 |
| 75 | ActualQty_WithLowYield_ShouldBeLarger | ProductionService::ActualQty | 낮은 수율 | 높은 수율 대비 실 생산량 많음 | Normal | ⬜ 미실행 | 2026-06-12 |
| 76 | ActualQty_Example_100_01_ShouldBe1112 | ProductionService::ActualQty | 부족분 100, 수율 0.1 | 실 생산량 == 1112 | Normal | ⬜ 미실행 | 2026-06-12 |
| 77 | TotalTime_3sec_100ea_ShouldBe300 | ProductionService::TotalTime | avgTime=3, qty=100 | 총 생산시간 == 300 | Normal | ⬜ 미실행 | 2026-06-12 |
| 78 | TotalTime_ZeroAvgTime_ShouldBeZero | ProductionService::TotalTime | avgTime=0 | 총 생산시간 == 0 | Normal | ⬜ 미실행 | 2026-06-12 |
| 79 | CheckCompletion_EmptyQueue_ShouldDoNothing | ProductionService::CheckCompletion | 빈 큐 상태 | 아무 동작 없음, 예외 미발생 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 80 | CheckCompletion_ExactFinishTime_ShouldComplete | ProductionService::CheckCompletion | 현재 시각 == 완료 시각 | 생산 완료 처리됨 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 81 | CheckCompletion_OneSecondBeforeFinish_ShouldNotComplete | ProductionService::CheckCompletion | 완료 1초 전 | 완료 처리되지 않음 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 82 | CheckCompletion_AlreadyConfirmed_ShouldNotSaveAgain | ProductionService::CheckCompletion | 이미 CONFIRMED 상태 | Save 중복 호출 없음 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 83 | ActualQty_YieldOne_ShouldBeMinimum | ProductionService::ActualQty | 수율 == 1.0 | 실 생산량 최소값 | Abnormal | ⬜ 미실행 | 2026-06-12 |
| 84 | ActualQty_VeryLowYield_ShouldBeVeryLarge | ProductionService::ActualQty | 매우 낮은 수율 | 실 생산량 매우 큼 | Abnormal | ⬜ 미실행 | 2026-06-12 |
