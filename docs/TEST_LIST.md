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
| 39 | GetAvailableStock_ReturnsCurrentMinusReserved | InventoryService::GetAvailableStock | currentStock=100, reservedStock=30 | availableStock == 70 | Normal | ✅ PASS | 2026-06-12 |
| 40 | AllocateStock_IncreasesReservedStock | InventoryService::AllocateStock | 가용 재고 내 수량 예약 | reservedStock 증가 | Normal | ✅ PASS | 2026-06-12 |
| 41 | AddStock_IncreasesCurrentStock | InventoryService::AddStock | 재고 추가 | currentStock 증가 | Normal | ✅ PASS | 2026-06-12 |
| 42 | DeductStock_DecreasesCurrentStock | InventoryService::DeductStock | 재고 차감 | currentStock 감소 | Normal | ✅ PASS | 2026-06-12 |
| 43 | GetInventory_ReturnsInventoryFromRepository | InventoryService::GetInventory | 존재하는 sampleId | Inventory 반환, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 44 | AllocateStock_ExactlyAvailableStock_ShouldSucceed | InventoryService::AllocateStock | qty == availableStock (경계) | 예외 미발생 | Normal | ✅ PASS | 2026-06-12 |
| 45 | AllocateStock_ExceedsAvailable_ShouldThrow | InventoryService::AllocateStock | qty > availableStock | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 46 | AllocateStock_ZeroAvailableStock_ShouldThrow | InventoryService::AllocateStock | availableStock == 0 | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 47 | AllocateStock_ZeroQty_ShouldThrow | InventoryService::AllocateStock | qty == 0 | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 48 | AddStock_ZeroQty_ShouldThrow | InventoryService::AddStock | qty == 0 | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 49 | DeductStock_ZeroQty_ShouldThrow | InventoryService::DeductStock | qty == 0 | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 50 | GetInventory_NonExistingSampleId_ShouldThrow | InventoryService::GetInventory | 존재하지 않는 sampleId | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 51 | GetAvailableStock_NonExistingSampleId_ShouldReturnZero | InventoryService::GetAvailableStock | 재고 레코드 없음 | 0 반환 (재고 부족 경로로 분기) | Abnormal | ✅ PASS | 2026-06-12 |
| 52 | DeductStock_ExceedsCurrentStock_ShouldThrow | InventoryService::DeductStock | qty > currentStock | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |

**파일**: `tests/service/OrderServiceTest.cpp`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 53 | PlaceOrder_ValidInput_ShouldBeReserved | OrderService::PlaceOrder | 유효한 sampleId, 고객명, 수량 | 생성된 주문 status == RESERVED | Normal | ✅ PASS | 2026-06-12 |
| 54 | PlaceOrder_ValidInput_OrderIdFormatShouldMatch | OrderService::PlaceOrder | 유효한 입력 | orderId 형식 ORD- 로 시작 | Normal | ✅ PASS | 2026-06-12 |
| 55 | ApproveOrder_SufficientStock_ShouldConfirm | OrderService::ApproveOrder | RESERVED 주문, 재고 충분 | 주문 status == CONFIRMED | Normal | ✅ PASS | 2026-06-12 |
| 56 | ApproveOrder_SufficientStock_ShouldAllocateStock | OrderService::ApproveOrder | RESERVED 주문, 재고 충분 | AllocateStock 호출됨 | Normal | ✅ PASS | 2026-06-12 |
| 57 | ApproveOrder_InsufficientStock_ShouldProducing | OrderService::ApproveOrder | RESERVED 주문, 재고 부족 | 주문 status == PRODUCING | Normal | ✅ PASS | 2026-06-12 |
| 58 | ApproveOrder_InsufficientStock_ShouldCreateProductionJob | OrderService::ApproveOrder | RESERVED 주문, 재고 부족 | ProductionJob 생성 및 저장 | Normal | ✅ PASS | 2026-06-12 |
| 59 | RejectOrder_ShouldBeRejected | OrderService::RejectOrder | RESERVED 주문 거절 | 주문 status == REJECTED | Normal | ✅ PASS | 2026-06-12 |
| 60 | GetReservedOrders_ShouldReturnOnlyReservedOrders | OrderService::GetReservedOrders | RESERVED 주문 2건 존재 | size == 2, 모두 RESERVED | Normal | ✅ PASS | 2026-06-12 |
| 61 | ApproveOrder_ExactlyMatchStock_ShouldConfirm | OrderService::ApproveOrder | availableStock == quantity (경계) | status == CONFIRMED | Normal | ✅ PASS | 2026-06-12 |
| 62 | ApproveOrder_ReservedByOtherOrder_ShouldUseAvailableOnly | OrderService::ApproveOrder | currentStock=10, reservedStock=8, qty=5 | availableStock(2) 기준으로 PRODUCING | Normal | ✅ PASS | 2026-06-12 |
| 63 | ApproveOrder_OneShortOfStock_ShouldProducing | OrderService::ApproveOrder | availableStock = quantity-1 (경계) | status == PRODUCING | Normal | ✅ PASS | 2026-06-12 |
| 64 | PlaceOrder_InvalidSampleId_ShouldThrow | OrderService::PlaceOrder | 존재하지 않는 sampleId | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 65 | PlaceOrder_EmptySampleId_ShouldThrow | OrderService::PlaceOrder | 빈 sampleId | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 66 | PlaceOrder_EmptyCustomerName_ShouldThrow | OrderService::PlaceOrder | 빈 customerName | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 67 | PlaceOrder_ZeroQuantity_ShouldThrow | OrderService::PlaceOrder | quantity == 0 | invalid_argument throw | Abnormal | ✅ PASS | 2026-06-12 |
| 68 | ApproveOrder_NotReserved_ShouldThrow | OrderService::ApproveOrder | RESERVED 아닌 상태(CONFIRMED) | logic_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 69 | ApproveOrder_AlreadyRejected_ShouldThrow | OrderService::ApproveOrder | REJECTED 상태 주문 승인 시도 | logic_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 70 | RejectOrder_NotReserved_ShouldThrow | OrderService::RejectOrder | PRODUCING 상태 주문 거절 시도 | logic_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 71 | ApproveOrder_OrderNotFound_ShouldThrow | OrderService::ApproveOrder | 존재하지 않는 orderId | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |
| 72 | RejectOrder_OrderNotFound_ShouldThrow | OrderService::RejectOrder | 존재하지 않는 orderId | runtime_error throw | Abnormal | ✅ PASS | 2026-06-12 |

---

## Phase 4 — 생산 큐 및 생산 서비스

**파일**: `tests/domain/ProductionQueueTest.cpp`  
**Fixture**: `ProductionQueueFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 73 | Enqueue_ShouldAddToQueue | ProductionQueue::Enqueue | Job 1건 추가 | 큐에 Job이 존재함 | Normal | ✅ PASS | 2026-06-12 |
| 74 | Peek_ReturnsFirstJob | ProductionQueue::Peek | Job 추가 후 Peek | 첫 번째 Job 반환 | Normal | ✅ PASS | 2026-06-12 |
| 75 | Dequeue_RemovesFirstJob | ProductionQueue::Dequeue | Job 1건 추가 후 Dequeue | 큐가 비어있음 | Normal | ✅ PASS | 2026-06-12 |
| 76 | FIFO_Order_ShouldBePreserved | ProductionQueue | Job 복수 추가 | 추가 순서대로 반환됨 | Normal | ✅ PASS | 2026-06-12 |
| 77 | IsEmpty_WhenNoJobs_ShouldBeTrue | ProductionQueue::IsEmpty | 빈 큐 상태 | IsEmpty() == true | Normal | ✅ PASS | 2026-06-12 |
| 78 | GetWaitingJobs_ShouldReturnAllJobsInFIFOOrder | ProductionQueue::GetWaitingJobs | Job 복수 추가 | FIFO 순서로 전체 Job 반환 | Normal | ✅ PASS | 2026-06-12 |
| 79 | GetWaitingJobs_AfterDequeue_ShouldReflectRemainingJobs | ProductionQueue::GetWaitingJobs | Dequeue 후 조회 | 남은 Job만 반환 | Normal | ✅ PASS | 2026-06-12 |
| 80 | Peek_WhenEmpty_ShouldReturnNullopt | ProductionQueue::Peek | 빈 큐에서 Peek | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 81 | Dequeue_WhenEmpty_ShouldNotThrow | ProductionQueue::Dequeue | 빈 큐에서 Dequeue | 예외 미발생 | Abnormal | ✅ PASS | 2026-06-12 |
| 82 | GetWaitingJobs_WhenEmpty_ShouldReturnEmptyVector | ProductionQueue::GetWaitingJobs | 빈 큐 상태 | 빈 vector 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 83 | Dequeue_AllJobs_ShouldBecomeEmpty | ProductionQueue::Dequeue | 전체 Job Dequeue | IsEmpty() == true | Abnormal | ✅ PASS | 2026-06-12 |

**파일**: `tests/service/ProductionServiceTest.cpp`  
**Fixture**: `ProductionServiceFixture`, `ProductionCalcFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 84 | CheckCompletion_BeforeFinishTime_ShouldNotComplete | ProductionService::CheckCompletion | 완료 시각 이전 | 주문 상태 변경 없음 | Normal | ✅ PASS | 2026-06-12 |
| 85 | CheckCompletion_AfterFinishTime_ShouldComplete | ProductionService::CheckCompletion | 완료 시각 이후 | 생산 완료 처리됨 | Normal | ✅ PASS | 2026-06-12 |
| 86 | OnComplete_ShouldUpdateOrderToConfirmed | ProductionService::OnComplete | 생산 완료 시 | 주문 status == CONFIRMED | Normal | ✅ PASS | 2026-06-12 |
| 87 | OnComplete_ShouldAddStockToInventory | ProductionService::OnComplete | 생산 완료 시 | 재고 증가 확인 | Normal | ✅ PASS | 2026-06-12 |
| 88 | OnComplete_ShouldDequeueJob | ProductionService::OnComplete | 생산 완료 시 | 큐에서 해당 Job 제거됨 | Normal | ✅ PASS | 2026-06-12 |
| 89 | MultipleJobs_ShouldProcessInFIFOOrder | ProductionService | Job 복수 등록 | 등록 순서대로 완료 처리됨 | Normal | ✅ PASS | 2026-06-12 |
| 90 | GetCurrentJob_NoJobs_ShouldReturnNullopt | ProductionService::GetCurrentJob | 빈 큐 상태 | nullopt 반환 | Normal | ✅ PASS | 2026-06-12 |
| 91 | GetCurrentJob_OneJob_ShouldReturnJob | ProductionService::GetCurrentJob | Job 1건 등록 | 해당 Job 반환 | Normal | ✅ PASS | 2026-06-12 |
| 92 | GetWaitingJobs_NoJobs_ShouldReturnEmpty | ProductionService::GetWaitingJobs | 빈 큐 상태 | 빈 vector 반환 | Normal | ✅ PASS | 2026-06-12 |
| 93 | GetWaitingJobs_TwoJobs_ShouldReturnSecondOnly | ProductionService::GetWaitingJobs | Job 2건 등록 | 두 번째 Job만 반환 | Normal | ✅ PASS | 2026-06-12 |
| 94 | ActualQty_WithHighYield_ShouldBeSmaller | ProductionService::ActualQty | 높은 수율 | 낮은 수율 대비 실 생산량 적음 | Normal | ✅ PASS | 2026-06-12 |
| 95 | ActualQty_WithLowYield_ShouldBeLarger | ProductionService::ActualQty | 낮은 수율 | 높은 수율 대비 실 생산량 많음 | Normal | ✅ PASS | 2026-06-12 |
| 96 | ActualQty_Example_100_01_ShouldBe1112 | ProductionService::ActualQty | 부족분 100, 수율 0.1 | 실 생산량 == 1112 | Normal | ✅ PASS | 2026-06-12 |
| 97 | TotalTime_3sec_100ea_ShouldBe300 | ProductionService::TotalTime | avgTime=3, qty=100 | 총 생산시간 == 300 | Normal | ✅ PASS | 2026-06-12 |
| 98 | TotalTime_ZeroAvgTime_ShouldBeZero | ProductionService::TotalTime | avgTime=0 | 총 생산시간 == 0 | Normal | ✅ PASS | 2026-06-12 |
| 99 | CheckCompletion_EmptyQueue_ShouldDoNothing | ProductionService::CheckCompletion | 빈 큐 상태 | 아무 동작 없음, 예외 미발생 | Abnormal | ✅ PASS | 2026-06-12 |
| 100 | CheckCompletion_ExactFinishTime_ShouldComplete | ProductionService::CheckCompletion | 현재 시각 == 완료 시각 | 생산 완료 처리됨 | Abnormal | ✅ PASS | 2026-06-12 |
| 101 | CheckCompletion_OneSecondBeforeFinish_ShouldNotComplete | ProductionService::CheckCompletion | 완료 1초 전 | 완료 처리되지 않음 | Abnormal | ✅ PASS | 2026-06-12 |
| 102 | CheckCompletion_AlreadyConfirmed_ShouldNotSaveAgain | ProductionService::CheckCompletion | 이미 CONFIRMED 상태 | Save 중복 호출 없음 | Abnormal | ✅ PASS | 2026-06-12 |
| 103 | ActualQty_YieldOne_ShouldBeMinimum | ProductionService::ActualQty | 수율 == 1.0 | 실 생산량 최소값 | Abnormal | ✅ PASS | 2026-06-12 |
| 104 | ActualQty_VeryLowYield_ShouldBeVeryLarge | ProductionService::ActualQty | 매우 낮은 수율 | 실 생산량 매우 큼 | Abnormal | ✅ PASS | 2026-06-12 |

---

## Phase 5 — 모니터링 및 출고 처리

**파일**: `tests/service/MonitoringServiceTest.cpp`  
**Fixture**: `MonitoringServiceFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 105 | GetOrderSummary_ShouldExcludeRejected | MonitoringService::GetOrderSummary | REJECTED 주문 존재 | REJECTED 집계 제외, FindByStatus(REJECTED) 미호출 | Normal | ✅ PASS | 2026-06-12 |
| 106 | GetOrderSummary_CountsCorrectByStatus | MonitoringService::GetOrderSummary | 각 상태별 주문 복수 | 상태별 카운트 정확히 일치 | Normal | ✅ PASS | 2026-06-12 |
| 107 | GetInventoryStatus_ZeroStock_ShouldBeDepleted | MonitoringService::GetInventoryStatus | currentStock == 0 | StockStatus::DEPLETED | Normal | ✅ PASS | 2026-06-12 |
| 108 | GetInventoryStatus_InsufficientAvailable_ShouldBeShortage | MonitoringService::GetInventoryStatus | availableStock == 0 (reserved >= current) | StockStatus::SHORTAGE | Normal | ✅ PASS | 2026-06-12 |
| 109 | GetInventoryStatus_SufficientAvailable_ShouldBeSufficient | MonitoringService::GetInventoryStatus | availableStock > 0 | StockStatus::SUFFICIENT | Normal | ✅ PASS | 2026-06-12 |
| 110 | GetInventoryStatus_OneUnitAvailable_ShouldBeSufficient | MonitoringService::GetInventoryStatus | availableStock == 1 (경계) | StockStatus::SUFFICIENT | Normal | ✅ PASS | 2026-06-12 |
| 111 | GetInventoryStatus_ReservedExceedsStock_ShouldBeShortage | MonitoringService::GetInventoryStatus | reservedStock > currentStock | availableStock < 0 → SHORTAGE | Abnormal | ✅ PASS | 2026-06-12 |
| 112 | GetInventoryStatus_NoSamples_ShouldReturnEmpty | MonitoringService::GetInventoryStatus | 시료 없음 | 빈 목록 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 113 | GetOrderSummary_NoOrders_ShouldReturnAllZero | MonitoringService::GetOrderSummary | 주문 없음 | 모든 카운트 == 0 | Abnormal | ✅ PASS | 2026-06-12 |
| 114 | GetInventoryStatus_NoInventoryRecord_ShouldBeDepleted | MonitoringService::GetInventoryStatus | 재고 레코드 없음 | currentStock=0 처리 → DEPLETED | Abnormal | ✅ PASS | 2026-06-12 |
| 115 | GetInventoryStatus_MultipleSamples_ShouldReturnCorrectStatuses | MonitoringService::GetInventoryStatus | S-010(DEPLETED), S-011(SHORTAGE), S-012(SUFFICIENT) | 각 시료별 올바른 StockStatus | Normal | ✅ PASS | 2026-06-12 |

**파일**: `tests/service/ReleaseServiceTest.cpp`  
**Fixture**: `ReleaseServiceFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 116 | GetConfirmedOrders_ShouldReturnOnlyConfirmed | ReleaseService::GetConfirmedOrders | CONFIRMED 주문 2건 | 반환 size == 2, 모두 CONFIRMED | Normal | ✅ PASS | 2026-06-12 |
| 117 | ReleaseOrder_ShouldChangeStatusToRelease | ReleaseService::ReleaseOrder | CONFIRMED 주문 출고 처리 | status == RELEASE 로 저장 | Normal | ✅ PASS | 2026-06-12 |
| 118 | ReleaseOrder_ShouldDeductStock | ReleaseService::ReleaseOrder | CONFIRMED 주문, currentStock=50, qty=10 | currentStock == 40 (50-10) | Normal | ✅ PASS | 2026-06-12 |
| 119 | ReleaseOrder_ShouldDeallocateReservedStock | ReleaseService::ReleaseOrder | reservedStock=10, qty=10 | reservedStock == 0 (10-10) | Normal | ✅ PASS | 2026-06-12 |
| 120 | ReleaseOrder_NotConfirmed_ShouldThrow | ReleaseService::ReleaseOrder | RESERVED 상태 주문 출고 시도 | logic_error throw, Save 미호출 | Abnormal | ✅ PASS | 2026-06-12 |
| 121 | ReleaseOrder_NotFound_ShouldThrow | ReleaseService::ReleaseOrder | 존재하지 않는 orderId | runtime_error throw, Save 미호출 | Abnormal | ✅ PASS | 2026-06-12 |

---

## Phase 6 — 데이터 영속성 (JSON)

**파일**: `tests/persistence/SampleJsonRepositoryTest.cpp`  
**Fixture**: `SampleJsonRepositoryFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 122 | Save_ShouldPersistToFile | SampleJsonRepository::Save | 시료 저장 | JSON 파일 생성됨 | Normal | ✅ PASS | 2026-06-12 |
| 123 | Load_ShouldRestoreFromFile | SampleJsonRepository 생성자 | 저장 후 새 인스턴스 생성 | 동일 데이터 복원 | Normal | ✅ PASS | 2026-06-12 |
| 124 | FindById_AfterSave_ShouldReturnSample | SampleJsonRepository::FindById | 저장 후 조회 | 저장된 시료 반환, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 125 | FindById_NotFound_ShouldReturnNullopt | SampleJsonRepository::FindById | 존재하지 않는 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 126 | FindById_EmptyId_ShouldReturnNullopt | SampleJsonRepository::FindById | 빈 문자열 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 127 | FindAll_ShouldReturnAllSaved | SampleJsonRepository::FindAll | 3건 저장 | size == 3 | Normal | ✅ PASS | 2026-06-12 |
| 128 | FindAll_EmptyStore_ShouldReturnEmptyVector | SampleJsonRepository::FindAll | 빈 저장소 | 빈 vector 반환 | Normal | ✅ PASS | 2026-06-12 |
| 129 | FindAll_AfterReload_ShouldReturnAllSaved | SampleJsonRepository::FindAll | 재로드 후 조회 | 저장된 수와 동일 | Normal | ✅ PASS | 2026-06-12 |
| 130 | FindByName_ShouldReturnMatchingByIdOrName | SampleJsonRepository::FindByName | 이름/ID 키워드 검색 | 매칭 시료만 반환 | Normal | ✅ PASS | 2026-06-12 |
| 131 | FindByName_NoMatch_ShouldReturnEmptyVector | SampleJsonRepository::FindByName | 매칭 없는 키워드 | 빈 vector 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 132 | ExistsById_AfterSave_ShouldReturnTrue | SampleJsonRepository::ExistsById | 저장 후 조회 | true 반환 | Normal | ✅ PASS | 2026-06-12 |
| 133 | ExistsById_NotSaved_ShouldReturnFalse | SampleJsonRepository::ExistsById | 미저장 ID | false 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 134 | Save_OverwriteSameId_ShouldRetainLatestValue | SampleJsonRepository::Save | 동일 ID 2회 저장 | 최신 값 유지, FindAll size == 1 | Normal | ✅ PASS | 2026-06-12 |

**파일**: `tests/persistence/OrderJsonRepositoryTest.cpp`  
**Fixture**: `OrderJsonRepositoryFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 135 | Save_ShouldPersistToFile | OrderJsonRepository::Save | 주문 저장 | JSON 파일 생성됨 | Normal | ✅ PASS | 2026-06-12 |
| 136 | FindById_AfterSave_ShouldReturnOrder | OrderJsonRepository::FindById | 저장 후 조회 | 저장된 주문 반환, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 137 | FindById_NotFound_ShouldReturnNullopt | OrderJsonRepository::FindById | 존재하지 않는 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 138 | FindById_EmptyId_ShouldReturnNullopt | OrderJsonRepository::FindById | 빈 문자열 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 139 | Load_ShouldRestoreFromFile | OrderJsonRepository 생성자 | 저장 후 새 인스턴스 생성 | 동일 데이터 복원 | Normal | ✅ PASS | 2026-06-12 |
| 140 | FindByStatus_ShouldReturnMatchingOrders | OrderJsonRepository::FindByStatus | 각 상태별 주문 저장 | 상태별 주문만 반환 | Normal | ✅ PASS | 2026-06-12 |
| 141 | FindByStatus_NoMatch_ShouldReturnEmptyVector | OrderJsonRepository::FindByStatus | 해당 상태 주문 없음 | 빈 vector 반환 | Normal | ✅ PASS | 2026-06-12 |
| 142 | FindAll_ShouldReturnAllSaved | OrderJsonRepository::FindAll | 3건 저장 | size == 3 | Normal | ✅ PASS | 2026-06-12 |
| 143 | FindAll_EmptyStore_ShouldReturnEmptyVector | OrderJsonRepository::FindAll | 빈 저장소 | 빈 vector 반환 | Normal | ✅ PASS | 2026-06-12 |
| 144 | FindAll_AfterReload_ShouldReturnAllSaved | OrderJsonRepository::FindAll | 재로드 후 조회 | 저장된 수와 동일 | Normal | ✅ PASS | 2026-06-12 |
| 145 | Save_OverwriteSameId_ShouldRetainLatestStatus | OrderJsonRepository::Save | 동일 ID 2회 저장(상태 변경) | 최신 상태 유지, size == 1 | Normal | ✅ PASS | 2026-06-12 |
| 146 | FindByStatus_AllStatusTypes_ShouldBeRestoredCorrectly | OrderJsonRepository::FindByStatus | 5가지 상태 전부 저장 후 재로드 | 각 상태별 1건씩 정확히 복원 | Normal | ✅ PASS | 2026-06-12 |

**파일**: `tests/persistence/InventoryJsonRepositoryTest.cpp`  
**Fixture**: `InventoryJsonRepositoryFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 147 | Save_ShouldPersistToFile | InventoryJsonRepository::Save | 재고 저장 | JSON 파일 생성됨 | Normal | ✅ PASS | 2026-06-12 |
| 148 | FindBySampleId_AfterSave_ShouldReturn | InventoryJsonRepository::FindBySampleId | 저장 후 조회 | 저장된 재고 반환, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 149 | ExistsBySampleId_AfterSave_ShouldReturnTrue | InventoryJsonRepository::ExistsBySampleId | 저장/미저장 ID | 저장=true, 미저장=false | Normal | ✅ PASS | 2026-06-12 |
| 150 | Load_ShouldRestoreFromFile | InventoryJsonRepository 생성자 | 저장 후 새 인스턴스 생성 | 동일 데이터 복원 | Normal | ✅ PASS | 2026-06-12 |
| 151 | FindBySampleId_NotFound_ShouldReturnNullopt | InventoryJsonRepository::FindBySampleId | 존재하지 않는 sampleId | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 152 | Save_DuplicateSampleId_ShouldOverwrite | InventoryJsonRepository::Save | 동일 sampleId 2회 저장 | 두 번째 값으로 덮어써짐 | Abnormal | ✅ PASS | 2026-06-12 |
| 153 | Save_ZeroStocks_ShouldPersistCorrectly | InventoryJsonRepository::Save | currentStock=0, reservedStock=0 | 정상 저장 및 조회, availableStock==0 | Normal | ✅ PASS | 2026-06-12 |

**파일**: `tests/persistence/ProductionJobJsonRepositoryTest.cpp`  
**Fixture**: `ProductionJobJsonRepositoryFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 154 | Save_ShouldPersistToFile | ProductionJobJsonRepository::Save | Job 저장 | JSON 파일 생성됨 | Normal | ✅ PASS | 2026-06-12 |
| 155 | FindById_AfterSave_ShouldReturn | ProductionJobJsonRepository::FindById | 저장 후 조회 | 저장된 Job 반환, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 156 | FindByOrderId_ShouldReturnMatchingJobs | ProductionJobJsonRepository::FindByOrderId | 동일 orderId 2건 저장 | 해당 orderId Job만 반환 | Normal | ✅ PASS | 2026-06-12 |
| 157 | FindAll_ShouldReturnAllSaved | ProductionJobJsonRepository::FindAll | 3건 저장 | size == 3 | Normal | ✅ PASS | 2026-06-12 |
| 158 | Load_ShouldRestoreFromFile | ProductionJobJsonRepository 생성자 | 저장 후 새 인스턴스 생성 | 동일 데이터 복원 | Normal | ✅ PASS | 2026-06-12 |
| 159 | FindById_NotFound_ShouldReturnNullopt | ProductionJobJsonRepository::FindById | 미존재 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 160 | FindByOrderId_NoMatch_ShouldReturnEmpty | ProductionJobJsonRepository::FindByOrderId | 매칭 없는 orderId | 빈 vector 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 161 | FindAll_EmptyRepository_ShouldReturnEmpty | ProductionJobJsonRepository::FindAll | 빈 저장소 | 빈 vector 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 162 | Save_DuplicateId_ShouldOverwrite | ProductionJobJsonRepository::Save | 동일 productionId 2회 저장 | 두 번째 값으로 덮어써짐 | Abnormal | ✅ PASS | 2026-06-12 |
| 163 | Save_ZeroProductionTime_ShouldPersistCorrectly | ProductionJobJsonRepository::Save | totalProductionTime=0, startTime==finishTime | 정상 저장 및 조회 | Normal | ✅ PASS | 2026-06-12 |

---

## Phase 7 — InMemory Repository 확장 커버리지

**파일**: `tests/repository/InMemoryRepositoryExtTest.cpp`  
**Fixture**: `InMemoryOrderRepositoryExtFixture`, `InMemoryProductionRepositoryExtFixture`, `InMemoryInventoryRepositoryExtFixture`

| # | 테스트명 | 대상 클래스/함수 | 조건 | 기대 결과 | 구분 | 상태 | 추가일 |
|---|---|---|---|---|---|---|---|
| 164 | FindAll_EmptyStore_ShouldReturnEmptyVector | InMemoryOrderRepository::FindAll | 빈 저장소 | 빈 vector 반환 | Normal | ✅ PASS | 2026-06-12 |
| 165 | FindAll_MultipleOrders_ShouldReturnAllOrders | InMemoryOrderRepository::FindAll | 3건 저장 | size == 3 | Normal | ✅ PASS | 2026-06-12 |
| 166 | FindAll_SingleOrder_ShouldContainSavedOrder | InMemoryOrderRepository::FindAll | 1건 저장 | 해당 주문 포함, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 167 | FindAll_AfterUpdate_ShouldReflectLatestStatus | InMemoryOrderRepository::FindAll | 상태 갱신 후 조회 | 최신 status 반환 | Normal | ✅ PASS | 2026-06-12 |
| 168 | FindAll_AllStatusTypes_ShouldReturnAll | InMemoryOrderRepository::FindAll | 5가지 상태 혼재 | size == 5 | Normal | ✅ PASS | 2026-06-12 |
| 169 | FindById_NotFound_ShouldReturnNullopt | InMemoryProductionRepository::FindById | 미존재 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 170 | FindById_EmptyId_ShouldReturnNullopt | InMemoryProductionRepository::FindById | 빈 문자열 ID | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 171 | FindAll_EmptyStore_ShouldReturnEmptyVector | InMemoryProductionRepository::FindAll | 빈 저장소 | 빈 vector 반환 | Normal | ✅ PASS | 2026-06-12 |
| 172 | FindAll_MultipleJobs_ShouldReturnAllJobs | InMemoryProductionRepository::FindAll | 3건 저장 | size == 3 | Normal | ✅ PASS | 2026-06-12 |
| 173 | FindAll_SingleJob_ShouldContainSavedJob | InMemoryProductionRepository::FindAll | 1건 저장 | 해당 Job 포함, 값 일치 | Normal | ✅ PASS | 2026-06-12 |
| 174 | FindAll_AfterUpdate_ShouldReflectUpdatedJob | InMemoryProductionRepository::FindAll | Job 갱신 후 조회 | 최신 sampleId 반환 | Normal | ✅ PASS | 2026-06-12 |
| 175 | FindAll_SameOrderId_MultipleJobs_ShouldReturnAll | InMemoryProductionRepository::FindAll | 동일 orderId Job 2건 | size == 2 | Normal | ✅ PASS | 2026-06-12 |
| 176 | FindBySampleId_NotFound_ShouldReturnNullopt | InMemoryInventoryRepository::FindBySampleId | 미존재 sampleId | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 177 | FindBySampleId_EmptyId_ShouldReturnNullopt | InMemoryInventoryRepository::FindBySampleId | 빈 문자열 sampleId | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 178 | FindBySampleId_EmptyStore_ShouldReturnNullopt | InMemoryInventoryRepository::FindBySampleId | 빈 저장소 | nullopt 반환 | Abnormal | ✅ PASS | 2026-06-12 |
| 179 | FindBySampleId_ExistingSample_ShouldReturnCorrectInventory | InMemoryInventoryRepository::FindBySampleId | 저장된 항목 조회 | 정확한 값 반환, availableStock 일치 | Normal | ✅ PASS | 2026-06-12 |
| 180 | FindBySampleId_StockEqualReserved_AvailableShouldBeZero | InMemoryInventoryRepository::FindBySampleId | currentStock == reservedStock | availableStock() == 0 | Normal | ✅ PASS | 2026-06-12 |
| 181 | FindBySampleId_ZeroStock_ShouldReturnZeroAvailable | InMemoryInventoryRepository::FindBySampleId | currentStock == 0 | availableStock() == 0 | Normal | ✅ PASS | 2026-06-12 |
| 182 | ExistsBySampleId_ExistingSample_ShouldReturnTrue | InMemoryInventoryRepository::ExistsBySampleId | 저장된 항목 | true 반환 | Normal | ✅ PASS | 2026-06-12 |
| 183 | ExistsBySampleId_NotFound_ShouldReturnFalse | InMemoryInventoryRepository::ExistsBySampleId | 미저장 항목 | false 반환 | Abnormal | ✅ PASS | 2026-06-12 |
