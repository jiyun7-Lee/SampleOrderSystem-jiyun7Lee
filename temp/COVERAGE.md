# Coverage Report — Phase 3

측정 일시: 2026-06-12
게이트 기준: 90% (Phase 3 커밋 게이트)

## 전체 커버리지

| 항목 | 수치 |
|---|---|
| 전체 라인 커버리지 (원시) | 51.6% |
| 전체 라인 커버리지 (UI 예외 제외) | 85.7% |
| 측정 대상 라인 수 (전체) | 221줄 |
| 측정 대상 라인 수 (UI 예외 제외) | 133줄 |
| 커버된 라인 수 | 114줄 |
| 테스트 실행 결과 | 38/38 PASS |

> UI/콘솔 의존 파일(SampleController.cpp 49줄, SampleView.cpp 39줄)은 stdin 자동화 불가로 커버리지 산정에서 제외

## 파일별 결과

| 파일 | 라인 커버리지 | 커버드/전체 | 등급 | 비고 |
|------|------------|------------|------|------|
| Inventory.h | 100.0% | 3/3 | EXCELLENT | |
| IInventoryRepository.h | 100.0% | 1/1 | EXCELLENT | |
| IOrderRepository.h | 100.0% | 1/1 | EXCELLENT | |
| IProductionRepository.h | 100.0% | 1/1 | EXCELLENT | |
| ISampleRepository.h | 100.0% | 1/1 | EXCELLENT | |
| InMemorySampleRepository.h | 100.0% | 31/31 | EXCELLENT | |
| ITimeProvider.h | 100.0% | 1/1 | EXCELLENT | |
| SystemTimeProvider.h | 100.0% | 3/3 | EXCELLENT | |
| SampleService.cpp | 95.7% | 22/23 | EXCELLENT | |
| InMemoryInventoryRepository.h | 91.7% | 11/12 | EXCELLENT | |
| InMemoryOrderRepository.h | 74.2% | 23/31 | CAUTION | FindAll() 미호출 |
| InMemoryProductionRepository.h | 64.0% | 16/25 | CAUTION | FindAll() 및 FindById 실패 경로 미테스트 |
| SampleController.cpp | 0.0% | 0/49 | EXCLUDED | stdin 의존 UI — 자동화 테스트 제외 |
| SampleView.cpp | 0.0% | 0/39 | EXCLUDED | stdin 의존 UI — 자동화 테스트 제외 |

## 미커버 주요 구간

| 파일 | 미커버 라인 | 내용 | 사유 |
|------|------------|------|------|
| SampleService.cpp | 12 | name 공백 검증 분기 | 테스트 미작성 — 빈 name 케이스 미호출 |
| InMemoryInventoryRepository.h | 15 | DeleteById 메서드 본체 | 테스트 미작성 |
| InMemoryOrderRepository.h | 20-27 | FindAll() 전체 | FindAll() 미테스트 |
| InMemoryProductionRepository.h | 15 | FindById nullopt 반환 분기 | 실패 경로 미테스트 |
| InMemoryProductionRepository.h | 20-27 | FindAll() 전체 | FindAll() 미테스트 |
| SampleController.cpp | 전체 | stdin 의존 UI | stdin 의존 — 자동화 테스트 제외 |
| SampleView.cpp | 전체 | stdin 의존 UI | stdin 의존 — 자동화 테스트 제외 |

## 판정

| 기준 | 임계값 | 실제 (UI 제외) | 판정 |
|------|--------|---------------|------|
| Phase 3 커밋 게이트 | 90% | 85.7% | BLOCK |

**최종 판정: BLOCK** — UI 예외 제외 후 라인 커버리지 85.7%로 90% 기준 미달

### BLOCK 사유 및 보완 필요 항목

커버리지 90% 달성을 위해 아래 항목에 대한 추가 테스트가 필요합니다.

| 우선순위 | 파일 | 미커버 구간 | 예상 추가 커버리지 |
|---------|------|------------|-----------------|
| 1 | InMemoryOrderRepository.h | FindAll() (lines 20-27) | +6.0% |
| 2 | InMemoryProductionRepository.h | FindAll() (lines 20-27) + FindById nullopt (line 15) | +7.5% |
| 3 | InMemoryInventoryRepository.h | DeleteById (line 15) | +0.8% |
| 4 | SampleService.cpp | name 공백 검증 (line 12) | +0.8% |

위 항목 전부 커버 시 UI 제외 커버리지: 약 93.2% (목표 90% 초과)

## 이력

| 날짜 | 전체 커버리지 | UI 제외 커버리지 | 테스트 수 | 비고 |
|------|-------------|----------------|---------|------|
| 2026-06-12 | 83.6% | — | 28 | Phase 1 TDD REFACTOR 단계 측정 |
| 2026-06-12 | 51.8% | 85.7% | 38 | Phase 2 완료 후 측정 |
| 2026-06-12 | 51.6% | 85.7% | 38 | Phase 3 측정 — BLOCK (90% 미달) |
