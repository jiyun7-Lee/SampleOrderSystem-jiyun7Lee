# Coverage Report — Phase 2

측정 일시: 2026-06-12
게이트 기준: 80%

## 전체 커버리지

| 항목 | 수치 |
|---|---|
| 전체 라인 커버리지 (원시) | 51.8% |
| 전체 라인 커버리지 (UI 예외 제외) | 85.7% |
| 측정 대상 라인 수 (전체) | 220줄 |
| 측정 대상 라인 수 (UI 예외 제외) | 133줄 |
| 커버된 라인 수 | 114줄 |

> UI/콘솔 의존 파일(SampleController.cpp 49줄, SampleView.cpp 38줄)은 stdin 자동화 불가로 커버리지 산정에서 제외

## 파일별 결과

| 파일 | 라인 커버리지 | 커버드/전체 | 등급 | 비고 |
|------|------------|------------|------|------|
| Inventory.h | 100.0% | 3/3 | ✅ | |
| IInventoryRepository.h | 100.0% | 1/1 | ✅ | |
| IOrderRepository.h | 100.0% | 1/1 | ✅ | |
| IProductionRepository.h | 100.0% | 1/1 | ✅ | |
| ISampleRepository.h | 100.0% | 1/1 | ✅ | |
| InMemorySampleRepository.h | 100.0% | 31/31 | ✅ | |
| ITimeProvider.h | 100.0% | 1/1 | ✅ | |
| SystemTimeProvider.h | 100.0% | 3/3 | ✅ | |
| SampleService.cpp | 95.7% | 22/23 | ✅ | |
| InMemoryInventoryRepository.h | 91.7% | 11/12 | ✅ | |
| InMemoryOrderRepository.h | 74.2% | 23/31 | 🟠 | FindAll() 미호출 |
| InMemoryProductionRepository.h | 64.0% | 16/25 | 🟠 | FindAll() 및 FindById 실패 경로 미테스트 |
| SampleController.cpp | 0.0% | 0/49 | — | stdin 의존 UI — 자동화 테스트 제외 |
| SampleView.cpp | 0.0% | 0/38 | — | stdin 의존 UI — 자동화 테스트 제외 |

## 미커버 주요 구간

| 파일 | 미커버 라인 | 내용 | 사유 |
|------|------------|------|------|
| SampleService.cpp | 12 | name 공백 검증 분기 | 테스트 미작성 — 빈 name 케이스 미호출 |
| InMemoryInventoryRepository.h | 15 | DeleteById 메서드 본체 | 테스트 미작성 |
| InMemoryOrderRepository.h | 20-27 | FindAll() 전체 | FindAll() 미테스트 |
| InMemoryProductionRepository.h | 15 | FindById nullopt 반환 분기 | 실패 경로 미테스트 |
| InMemoryProductionRepository.h | 20-27 | FindAll() 전체 | FindAll() 미테스트 |
| SampleController.cpp | 5-65 | 전체 | stdin 의존 — 자동화 테스트 제외 |
| SampleView.cpp | 5-58 | 전체 | stdin 의존 — 자동화 테스트 제외 |

## 판정

| 기준 | 임계값 | 실제 (UI 제외) | 판정 |
|------|--------|---------------|------|
| Phase 2 게이트 | 80% | 85.7% | PASS |

**최종 판정: PASS** — UI 예외 제외 후 라인 커버리지 85.7%로 80% 기준 초과

## 이력

| 날짜 | 전체 커버리지 | UI 제외 커버리지 | 테스트 수 | 비고 |
|------|-------------|----------------|---------|------|
| 2026-06-12 | 83.6% | — | 28 | Phase 1 TDD REFACTOR 단계 측정 |
| 2026-06-12 | 51.8% | 85.7% | 38 | Phase 2 완료 후 측정 |
