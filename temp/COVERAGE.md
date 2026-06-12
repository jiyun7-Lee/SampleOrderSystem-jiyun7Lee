# SampleOrderSystem 코드 커버리지 현황

> 최종 측정: 2026-06-12 오늘
> 도구: OpenCppCoverage (C:\Program Files\OpenCppCoverage\OpenCppCoverage.exe)
> 빌드: Debug / 테스트: 28개 PASS
> 측정 대상: C:\reviewer\Semicon\Semicon\src

## 전체 커버리지

| 항목 | 수치 |
|---|---|
| 전체 라인 커버리지 | 83.6% |
| 측정 대상 라인 수 | 110줄 |
| 커버된 라인 수 | 92줄 |

## 파일별 커버리지

| 파일 | 경로 | 커버드/전체 | 라인 커버리지 | 등급 |
|---|---|---|---|---|
| Inventory.h | src/model/domain/ | 3/3 | 100.0% | |
| IInventoryRepository.h | src/model/repository/ | 1/1 | 100.0% | |
| IOrderRepository.h | src/model/repository/ | 1/1 | 100.0% | |
| IProductionRepository.h | src/model/repository/ | 1/1 | 100.0% | |
| ISampleRepository.h | src/model/repository/ | 1/1 | 100.0% | |
| InMemorySampleRepository.h | src/model/repository/ | 30/30 | 100.0% | |
| ITimeProvider.h | src/util/ | 1/1 | 100.0% | |
| SystemTimeProvider.h | src/util/ | 3/3 | 100.0% | |
| InMemoryInventoryRepository.h | src/model/repository/ | 11/12 | 91.7% | |
| InMemoryOrderRepository.h | src/model/repository/ | 23/31 | 74.2% | |
| InMemoryProductionRepository.h | src/model/repository/ | 16/25 | 64.0% | |

## 미커버 주요 구간

| 파일 | 미커버 라인 | 내용 | 사유 |
|---|---|---|---|
| InMemoryInventoryRepository.h | 15 | DeleteById 메서드 본체 | 테스트 미작성 — DeleteById 미호출 |
| InMemoryOrderRepository.h | 20-27 | FindAll() 전체 | FindAll() 미테스트 |
| InMemoryProductionRepository.h | 15, 20-27 | FindById nullopt 분기 + FindAll() | FindAll() 및 FindById 실패 경로 미테스트 |

### 미커버 구간 상세

**InMemoryOrderRepository.h (8줄 미커버)**
- 라인 20-27: `FindAll()` 메서드 — 현재 테스트에서 호출되지 않음

**InMemoryProductionRepository.h (9줄 미커버)**
- 라인 15: `FindById` 에서 `nullopt` 반환 분기
- 라인 20-27: `FindAll()` 메서드 — 현재 테스트에서 호출되지 않음

**InMemoryInventoryRepository.h (1줄 미커버)**
- 라인 15: `DeleteById` 메서드 본체

## 판정

| 기준 | 임계값 | 실제 | 판정 |
|---|---|---|---|
| Phase 1 게이트 | 80% | 83.6% | PASS |
| Phase 7 목표 | 80% | 83.6% | PASS |

**최종 판정: PASS** — 전체 라인 커버리지 83.6%로 80% 기준 초과

## 이력

| 날짜 | 전체 커버리지 | 테스트 수 | 비고 |
|---|---|---|---|
| 2026-06-12 | 83.6% | 28 | Phase 1 TDD REFACTOR 단계 측정 |
