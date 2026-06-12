# PLAN.md — 반도체 시료 생산주문관리 시스템

## 프로젝트 개요

- **언어**: C++17
- **아키텍처**: MVC + Service Layer + Repository Pattern
- **테스트**: Google Test + Google Mock (gmock)
- **빌드**: CMake / MSVC (`/utf-8`)
- **참조 문서**: `CLAUDE.md`, `PRD.md`

---

## Phase 구성 요약

| Phase | 내용 | 담당 에이전트 | 상세 |
|-------|------|-------------|------|
| Phase 1 | 프로젝트 기반 구조 및 도메인 모델 | `feature` | [Phase1_기반구조_도메인모델.md](Phase1_기반구조_도메인모델.md) |
| Phase 2 | 시료 관리 기능 (FR-01~03) | `feature` | [Phase2_시료관리.md](Phase2_시료관리.md) |
| Phase 3 | 주문 관리 기능 (FR-04~06) | `feature` | [Phase3_주문관리.md](Phase3_주문관리.md) |
| Phase 4 | 생산라인 관리 (FR-07~08) | `feature` | [Phase4_생산라인관리.md](Phase4_생산라인관리.md) |
| Phase 5 | 모니터링 및 출고 처리 (FR-09~11) | `feature` | [Phase5_모니터링_출고처리.md](Phase5_모니터링_출고처리.md) |
| Phase 6 | 데이터 영속성 (JSON 파일 기반) | `feature` | [Phase6_데이터영속성.md](Phase6_데이터영속성.md) |
| Phase 7 | 단위 테스트 및 커버리지 달성 | `unit-test` + `coverage` | [Phase7_단위테스트_커버리지.md](Phase7_단위테스트_커버리지.md) |

---

## Phase별 체크리스트

### Phase 1 — 프로젝트 기반 구조 및 도메인 모델 → [상세](Phase1_기반구조_도메인모델.md)
- [ ] Step 1-1: 디렉토리 구조 및 CMakeLists.txt 구성
- [ ] Step 1-2: 주문 상태 `enum class OrderStatus` 정의
- [ ] Step 1-3: 도메인 모델 클래스 구현 (Sample, Order, Inventory, ProductionJob)
- [ ] Step 1-4: Repository 인터페이스 정의 (ISampleRepository, IOrderRepository, IInventoryRepository, IProductionRepository)
- [ ] Step 1-5: ITimeProvider 인터페이스 및 SystemTimeProvider 구현
- [ ] Step 1-6: InMemory Repository 구현체 (테스트용)
- [ ] Step 1-7: Main 진입점 및 콘솔 한글 인코딩 설정

### Phase 2 — 시료 관리 기능 (FR-01~03) → [상세](Phase2_시료관리.md)
- [ ] Step 2-1: SampleService 구현 (시료 등록·조회·검색)
- [ ] Step 2-2: SampleController 구현
- [ ] Step 2-3: SampleView 구현 (콘솔 UI)
- [ ] Step 2-4: 메인 메뉴 [1] 시료 관리 연결

### Phase 3 — 주문 관리 기능 (FR-04~06) → [상세](Phase3_주문관리.md)
- [ ] Step 3-1: InventoryService 구현 (availableStock 계산)
- [ ] Step 3-2: OrderService 구현 (주문 접수·승인·거절)
- [ ] Step 3-3: OrderController 구현
- [ ] Step 3-4: OrderView 구현 (주문 접수 UI, 승인/거절 UI)
- [ ] Step 3-5: 메인 메뉴 [2] 시료 주문, [3] 주문 승인/거절 연결

### Phase 4 — 생산라인 관리 (FR-07~08) → [상세](Phase4_생산라인관리.md)
- [ ] Step 4-1: ProductionQueue 구현 (FIFO, `std::queue<ProductionJob>`)
- [ ] Step 4-2: ProductionService 구현 (실시간 생산 처리, 생산완료 감지)
- [ ] Step 4-3: 생산량·생산시간 계산 (`ceil(requiredQty / (yieldRate * 0.9))`)
- [ ] Step 4-4: 초과 생산분 즉시 availableStock 반영 로직
- [ ] Step 4-5: ProductionController 및 ProductionView 구현
- [ ] Step 4-6: 메인 메뉴 [5] 생산라인 조회 연결

### Phase 5 — 모니터링 및 출고 처리 (FR-09~11) → [상세](Phase5_모니터링_출고처리.md)
- [ ] Step 5-1: ReleaseService 구현 (CONFIRMED → RELEASE, 재고 차감)
- [ ] Step 5-2: MonitoringService 구현 (주문량·재고량·상태 조회)
- [ ] Step 5-3: ReleaseController / MonitoringController 구현
- [ ] Step 5-4: ReleaseView / MonitoringView 구현
- [ ] Step 5-5: 메인 메뉴 [4] 모니터링, [6] 출고 처리 연결

### Phase 6 — 데이터 영속성 (JSON 파일 기반) → [상세](Phase6_데이터영속성.md)
- [ ] Step 6-1: JSON 라이브러리 연동 (nlohmann/json)
- [ ] Step 6-2: SampleJsonRepository 구현
- [ ] Step 6-3: OrderJsonRepository 구현
- [ ] Step 6-4: InventoryJsonRepository 구현
- [ ] Step 6-5: ProductionJobJsonRepository 구현
- [ ] Step 6-6: 애플리케이션 시작 시 데이터 로드 / 변경 시 자동 저장

### Phase 7 — 단위 테스트 및 커버리지 달성 → [상세](Phase7_단위테스트_커버리지.md)
- [ ] Step 7-1: Domain 단위 테스트 (Sample, Order, Inventory, ProductionJob)
- [ ] Step 7-2: SampleService 단위 테스트 (gmock — ISampleRepository 주입)
- [ ] Step 7-3: OrderService 단위 테스트 (gmock — IOrderRepository, IInventoryRepository, ITimeProvider 주입)
- [ ] Step 7-4: ProductionService 단위 테스트 (gmock — ITimeProvider로 시간 시뮬레이션)
- [ ] Step 7-5: MonitoringService / ReleaseService 단위 테스트
- [ ] Step 7-6: 커버리지 측정 (OpenCppCoverage) — 목표 80% 이상

---

## 에이전트 실행 순서 (각 Phase 공통)

```
① unit-test    → gmock 기반 테스트 작성 (RED)
② git-manager  → [unitTest] 커밋 (사용자 confirm)
③ feature      → RED → GREEN 최소 구현
④ clean-code   → SOLID 검토
⑤ tester       → 빌드 + 전체 테스트 실행
⑥ coverage     → 80% 게이트 확인 (Phase 마지막 Step)
⑦ git-manager  → [feat] 커밋 (사용자 confirm)
```

---

## 디렉토리 구조 목표

```
Semicon/
├── src/
│   ├── main.cpp
│   ├── controller/
│   │   ├── SampleController.h / .cpp
│   │   ├── OrderController.h / .cpp
│   │   ├── ProductionController.h / .cpp
│   │   ├── MonitoringController.h / .cpp
│   │   └── ReleaseController.h / .cpp
│   ├── service/
│   │   ├── SampleService.h / .cpp
│   │   ├── OrderService.h / .cpp
│   │   ├── InventoryService.h / .cpp
│   │   ├── ProductionService.h / .cpp
│   │   ├── MonitoringService.h / .cpp
│   │   └── ReleaseService.h / .cpp
│   ├── model/
│   │   ├── domain/
│   │   │   ├── Sample.h
│   │   │   ├── Order.h
│   │   │   ├── Inventory.h
│   │   │   ├── ProductionJob.h
│   │   │   └── OrderStatus.h
│   │   └── repository/
│   │       ├── ISampleRepository.h
│   │       ├── IOrderRepository.h
│   │       ├── IInventoryRepository.h
│   │       └── IProductionRepository.h
│   ├── view/
│   │   ├── SampleView.h / .cpp
│   │   ├── OrderView.h / .cpp
│   │   ├── ProductionView.h / .cpp
│   │   ├── MonitoringView.h / .cpp
│   │   └── ReleaseView.h / .cpp
│   ├── persistence/
│   │   ├── SampleJsonRepository.h / .cpp
│   │   ├── OrderJsonRepository.h / .cpp
│   │   ├── InventoryJsonRepository.h / .cpp
│   │   └── ProductionJobJsonRepository.h / .cpp
│   └── util/
│       ├── ITimeProvider.h
│       └── SystemTimeProvider.h / .cpp
├── tests/
│   ├── domain/
│   ├── service/
│   ├── repository/
│   └── controller/
├── temp/
│   ├── PLAN.md
│   ├── Phase1_기반구조_도메인모델.md
│   ├── Phase2_시료관리.md
│   ├── Phase3_주문관리.md
│   ├── Phase4_생산라인관리.md
│   ├── Phase5_모니터링_출고처리.md
│   ├── Phase6_데이터영속성.md
│   ├── Phase7_단위테스트_커버리지.md
│   └── phase{N}_progress.md
├── docs/
│   ├── TEST_LIST.md
│   ├── FEATURE_LIST.md
│   ├── CHANGELOG.md
│   └── API.md
├── CMakeLists.txt
├── CLAUDE.md
└── PRD.md
```
