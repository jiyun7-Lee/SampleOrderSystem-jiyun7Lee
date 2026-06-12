# PRD — 00. 프로젝트 개요

> 상위 문서: [PRD.md](../../PRD.md)

---

## 1. 프로젝트 개요

### 배경

가상의 반도체 회사 "S-Semi"는 연구소, 팹리스(Fabless) 업체, 대학 연구실 등에 반도체 시료(Sample)를 납품한다.  
주문량 급증으로 엑셀·수기 관리의 한계가 드러나 체계적인 시스템이 필요해졌다.

### 목적

시료 등록, 주문 접수, 생산, 재고, 출고 과정을 통합 관리하는 **콘솔 기반 시스템** 개발.

### 개발 목표

- 콘솔(CLI) 기반 애플리케이션 구현
- MVC 아키텍처 + SOLID 원칙 적용
- 데이터 영속성 지원 (재시작 후에도 데이터 유지)
- 단위 테스트 작성 (코드 커버리지 80% 이상)
- Agentic Engineering 기반 개발
- Clean Code 원칙 준수

---

## 2. 역할 흐름

```
고객(시료 요청)
    ↓ 이메일로 요청
주문 담당자(주문서 작성/관리)
    ↓ 주문서 전달
생산 담당자(시료 생산·승인)
    ↓ 승인 / 거절
주문 담당자
```

---

## 3. 핵심 도메인 모델

### Sample (시료)

| 속성 | 타입 | 설명 |
|------|------|------|
| sampleId | String | 고유 식별자 (중복 불가) |
| name | String | 시료명 (공백 불가) |
| averageProductionTime | double | 평균 생산시간 (초/ea, 0 초과) |
| yieldRate | double | 수율 0.0 초과 ~ 1.0 이하 |

> 수율(yieldRate) = 정상 시료 수 / 총 생산 시료 수 (예: 0.92 = 92%)

### Order (주문)

| 속성 | 타입 | 설명 |
|------|------|------|
| orderId | String | 주문 번호 (예: ORD-20260416-0001) |
| sampleId | String | 참조 시료 ID |
| customerName | String | 고객명 |
| quantity | int | 주문 수량 |
| status | OrderStatus | 주문 상태 |
| createdAt | DateTime | 주문 생성 시각 |

### Inventory (재고)

| 속성 | 타입 | 필드명 | 설명 |
|------|------|--------|------|
| 현재 재고 | int | `currentStock` | 실제 생산 완료된 정상 시료 수량 |
| 필요 재고 | int | `reservedStock` | 이미 주문이 확보한 수량 |
| 주문 가능 재고 | int | `availableStock` | `currentStock - reservedStock` |

> 신규 주문 승인 여부는 반드시 **availableStock** 기준으로 판단한다.

### ProductionJob (생산 작업)

| 속성 | 타입 | 설명 |
|------|------|------|
| productionId | String | 생산 작업 ID |
| orderId | String | 연결된 주문 ID |
| sampleId | String | 시료 ID |
| requiredQuantity | int | 부족 수량 |
| actualProductionQuantity | int | 실 생산량 |
| totalProductionTime | double | 총 생산 시간 (초) |
| startTime | DateTime | 생산 시작 시각 |
| expectedFinishTime | DateTime | 생산 완료 예정 시각 |

---

## 4. 주문 상태 흐름

| 상태 | 의미 |
|------|------|
| `RESERVED` | 주문 접수 |
| `PRODUCING` | 주문 승인 완료 + 재고 부족 → 생산 중 |
| `CONFIRMED` | 주문 승인 완료 + 출고 대기 중 |
| `RELEASE` | 출고 완료 |
| `REJECTED` | 주문 거절 (모니터링 제외) |

### 상태 전이 다이어그램

```
RESERVED ──→ [재고 충분] ──────────────→ CONFIRMED ──→ RELEASE
         ↘ [재고 부족] ──→ PRODUCING ──→ CONFIRMED ──→ RELEASE
         ↘ [거절] ──→ REJECTED
```

### 중요 규칙

- 생산 필요 여부는 **주문 승인 시점에 단 한 번만** 판단한다.
- 생산 작업이 생성된 이후에는 재고 상황이 변경되어도 생산 여부를 재판단하지 않는다.
- 생산 작업은 취소되지 않는다.

---

## 5. 메인 메뉴 구성

```
[1] 시료 관리       시료 등록 / 목록 조회 / 이름 검색
[2] 시료 주문       고객 주문 접수 (RESERVED 생성)
[3] 주문 승인/거절  RESERVED 주문 목록 확인 후 승인·거절 처리
[4] 모니터링        상태별 주문 수 및 시료별 재고 현황
[5] 생산라인 조회   현재 생산 중 시료 및 대기 큐 확인
[6] 출고 처리       CONFIRMED 주문에 대해 출고 실행
[0] 종료
```
