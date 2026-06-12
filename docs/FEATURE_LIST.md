# Feature List

| # | Feature | 설명 | 구현(GREEN) | 리팩토링 | Clean-Code | 비고 |
|---|---|---|---|---|---|---|
| F-01 | 도메인 모델 정의 | Sample, Order, Inventory, ProductionJob 클래스 및 OrderStatus enum class 정의 | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 1 완료 (커밋 b709db7, d732b63) |
| F-02 | Repository 인터페이스 | ISampleRepository, IOrderRepository, IInventoryRepository, IProductionRepository 인터페이스 정의 | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 1 완료 — ISP 위반 제거, FindAll 추가 |
| F-03 | InMemory Repository 구현체 | InMemorySampleRepository, InMemoryOrderRepository, InMemoryInventoryRepository, InMemoryProductionRepository 구현 | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 1 완료 |
| F-04 | ITimeProvider / SystemTimeProvider | 시간 추상화 인터페이스 및 실제 시스템 시간 구현체 | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 1 완료 |
| F-05 | SampleService | 시료 등록(RegisterSample), 전체 조회(GetAllSamples), ID 조회(FindSampleById), 이름 검색(SearchByName) | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 2 완료 (커밋 8ec0a94), 커버리지 85.7% |
| F-06 | SampleController | 시료 관리 UI 흐름 제어 — 등록, 목록 조회, 검색 | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 2 완료 |
| F-07 | SampleView | 시료 등록 폼, 목록 출력, 검색 결과 출력 콘솔 UI | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 2 완료 |
| F-08 | 메인 메뉴 [1] 시료 관리 연결 | main.cpp 메인 메뉴에서 [1] 시료 관리 선택 시 SampleController 호출 | ✅ 2026-06-12 | ✅ 2026-06-12 | ✅ 2026-06-12 | Phase 2 완료 |
| F-09 | InventoryService | 가용 재고 조회(GetAvailableStock), 재고 예약(AllocateStock), 재고 추가(AddStock), 재고 차감(DeductStock), Inventory 조회(GetInventory) | ✅ 2026-06-12 | ⬜ | ⬜ | Phase 3 구현 완료 |
| F-10 | OrderService | 주문 접수(PlaceOrder, ORD-YYYYMMDD-NNNN), 주문 승인(ApproveOrder: 재고 충분→CONFIRMED / 부족→PRODUCING+ProductionJob), 주문 거절(RejectOrder), RESERVED 목록 조회(GetReservedOrders) | ✅ 2026-06-12 | ⬜ | ⬜ | Phase 3 구현 완료 |
| F-11 | OrderController | 주문 접수 UI 흐름(PlaceOrder), 승인/거절 UI 흐름(ProcessApproval) | ✅ 2026-06-12 | ⬜ | ⬜ | Phase 3 구현 완료 |
| F-12 | OrderView | 주문 입력 폼, 주문 목록 표시, 승인/거절 선택 UI | ✅ 2026-06-12 | ⬜ | ⬜ | Phase 3 구현 완료 |
| F-13 | 메인 메뉴 [2][3] 연결 | main.cpp 메인 메뉴에서 [2] 시료 주문 및 [3] 주문 승인/거절 선택 시 OrderController 호출 | ✅ 2026-06-12 | ⬜ | ⬜ | Phase 3 구현 완료 |
| F-14 | ProductionService | 생산라인 FIFO 스케줄링, 실 생산량 계산(ceil(부족분/(수율*0.9))), 생산완료 시 PRODUCING→CONFIRMED 전환 | ⬜ | ⬜ | ⬜ | Phase 4 예정 |
| F-15 | ProductionController / ProductionView | 생산라인 조회 UI | ⬜ | ⬜ | ⬜ | Phase 4 예정 |
| F-16 | 메인 메뉴 [5] 생산라인 조회 연결 | main.cpp 메인 메뉴에서 [5] 생산라인 조회 선택 시 ProductionController 호출 | ⬜ | ⬜ | ⬜ | Phase 4 예정 |
| F-17 | MonitoringService | 상태별 주문 수 집계(RESERVED/CONFIRMED/PRODUCING/RELEASE), 시료별 재고 현황(여유/부족/고갈) | ⬜ | ⬜ | ⬜ | Phase 5 예정 |
| F-18 | ReleaseService | CONFIRMED 주문 출고 처리(CONFIRMED→RELEASE), 재고 차감 | ⬜ | ⬜ | ⬜ | Phase 5 예정 |
| F-19 | MonitoringController / MonitoringView | 모니터링 UI | ⬜ | ⬜ | ⬜ | Phase 5 예정 |
| F-20 | ReleaseController / ReleaseView | 출고 처리 UI | ⬜ | ⬜ | ⬜ | Phase 5 예정 |
| F-21 | 메인 메뉴 [4][6] 연결 | main.cpp 메인 메뉴에서 [4] 모니터링, [6] 출고 처리 연결 | ⬜ | ⬜ | ⬜ | Phase 5 예정 |
| F-22 | 데이터 영속성 (JSON) | nlohmann/json 기반 파일 저장/로드 Repository 구현체 (Sample/Order/Inventory/ProductionJob) | ⬜ | ⬜ | ⬜ | Phase 6 예정 |
