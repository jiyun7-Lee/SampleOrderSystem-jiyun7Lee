# 반도체 시료 생산주문관리 시스템 (S-Semi)

## 프로젝트 개요

가상의 반도체 회사 "S-Semi"의 시료(Sample) 생산주문관리 시스템.
연구소, 팹리스(Fabless) 업체, 대학 연구실 등에 납품하는 반도체 시료의 주문/생산/출고를 통합 관리하는 **콘솔 기반** 애플리케이션.

## 아키텍처

- **패턴**: MVC (Model / Controller / View)
- **실행 방식**: 콘솔(CLI) 기반 — 담당자가 직접 명령을 입력
- **데이터 영속성**: 애플리케이션 재시작 후에도 데이터 유지 (파일/JSON/DB 중 선택)

## 역할별 흐름

```
고객(시료 요청) → 주문 담당자(주문서 작성/관리) → 생산 담당자(시료 등록/승인/거절)
                        ↑                                     |
                        └──────── 승인 / 거절 ────────────────┘
```

## 주문 상태 흐름

| 상태 | 의미 |
|------|------|
| `RESERVED` | 주문 접수 |
| `REJECTED` | 주문 거절 (모니터링 제외) |
| `PRODUCING` | 주문 승인 완료 + 재고 부족 → 생산 중 |
| `CONFIRMED` | 주문 승인 완료 + 출고 대기 중 |
| `RELEASE` | 출고 완료 |

**상태 전이 규칙**
- `RESERVED` → 승인(재고 충분) → `CONFIRMED`
- `RESERVED` → 승인(재고 부족) → `PRODUCING` → 생산완료 → `CONFIRMED`
- `RESERVED` → 거절 → `REJECTED`
- `CONFIRMED` → 출고 처리 → `RELEASE`

## 메인 메뉴 구성

```
[1] 시료 관리       - 시료 등록 / 목록 조회 / 이름 검색
[2] 시료 주문       - 고객 주문 접수 (RESERVED 생성)
[3] 주문 승인/거절  - RESERVED 주문 목록 확인 후 승인·거절 처리
[4] 모니터링        - 상태별 주문 수 및 시료별 재고 현황
[5] 생산라인 조회   - 현재 생산 중 시료 및 대기 큐 확인
[6] 출고 처리       - CONFIRMED 주문에 대해 출고 실행
[0] 종료
```

## 기능 명세

### 1. 시료 관리

시료(Sample)는 시스템의 기본 단위. **등록된 시료만 주문 가능**.

| 기능 | 설명 |
|------|------|
| 시료 등록 | 시료 ID, 이름, 평균 생산시간, 수율 입력 |
| 시료 조회 | 전체 목록 + 현재 재고 수량 표시 |
| 시료 검색 | 이름 등 속성으로 검색 |

- **수율**: 정상 시료 수 / 총 생산 시료 수 (예: 0.92 = 92%)

### 2. 시료 주문

고객이 시료를 요청하면 주문 담당자가 주문 생성.

- **입력값**: 시료 ID, 고객명, 주문 수량
- **생성 상태**: `RESERVED`
- **주문번호 형식**: `ORD-YYYYMMDD-NNNN`

### 3. 주문 승인/거절

`RESERVED` 상태 주문 목록을 확인하고 처리.

**승인 시 자동 분기**:
- 재고 충분 → 즉시 `CONFIRMED`
- 재고 부족 → 생산 라인에 자동 등록 + `PRODUCING`으로 전환

**거절**: 즉시 `REJECTED`로 전환

### 4. 모니터링

- **주문량 확인**: 상태별(`RESERVED` / `CONFIRMED` / `PRODUCING` / `RELEASE`) 주문 수 — `REJECTED` 제외
- **재고량 확인**: 시료별 현재 재고 + 상태 표기
  - `여유`: 주문 대비 재고 충분
  - `부족`: 주문 대비 재고 부족
  - `고갈`: 수량 0

### 5. 생산 라인

주문량 부족분 생산 시 수율·오차 고려.

```
실 생산량 = ceil(부족분 / (수율 * 0.9))
총 생산 시간 = 평균 생산시간 * 실 생산량
```

- 생산 완료 시: `PRODUCING` → `CONFIRMED`
- **스케줄링 전략**: FIFO (선입선출)
- **생산 라인**: 단일 라인, 시료를 하나씩 순차 생산

### 6. 출고 처리

- `CONFIRMED` 상태 주문 목록 표시
- 선택한 주문 출고 실행 → `RELEASE`로 전환

## 데이터 모델

### Sample (시료)
```
- id: String (예: S-001)
- name: String
- avgProductionTime: double (분/ea)
- yield: double (0.0 ~ 1.0)
- stock: int (현재 재고)
```

### Order (주문)
```
- orderId: String (예: ORD-20260416-0001)
- sampleId: String
- customerName: String
- quantity: int
- status: OrderStatus (RESERVED / REJECTED / PRODUCING / CONFIRMED / RELEASE)
- createdAt: DateTime
```

## 개발 주안점 (미션 요구사항)

1. **CLAUDE.md, PRD.md 등 문서 관리** 철저
2. **Harness 도입** (Claude Code 활용)
3. **테스트 코드** 작성
4. **Clean Code** 원칙 준수
5. **Commit 이력** 관리

---

## Agent 구성 (`.claude/agents/`)

이 프로젝트는 TDD 사이클을 자동화하기 위한 11개의 전문 에이전트를 사용한다.  
에이전트 파일 위치: `.claude/agents/`

### 사이클 흐름

```
director
  └▶ tdd-cycle-checker
        ① unit-test       → gmock 기반 테스트 작성 (RED)
        ①- docs-writer    → TEST_LIST.md 갱신
        ② git-manager     → [unitTest] 커밋 (사용자 confirm 필수)
        ③ feature         → RED → GREEN 최소 구현
           refactoring    → 함수/타입 수준 개선 (Phase 1·2)
           clean-code     → SOLID 적용 / 클래스 분리 (Phase 3)
        ③- docs-writer    → FEATURE_LIST.md 갱신
        ④ clean-code      → SOLID 검토
        ⑤ tester          → 빌드 + GTest 실행 → PASS/FAIL 보고
        ⑥ coverage        → 90% 커밋 게이트
        ⑦ git-manager     → [feat]/[refactor] 커밋
        ⑦- docs-writer    → CHANGELOG·TEST_LIST 갱신
```

### 에이전트 역할 요약

| 에이전트 | 파일 | 역할 | 수정 가능 파일 |
|---------|------|------|---------------|
| `director` | `director.md` | TDD 사이클 총괄 지휘, PLAN.md 기준으로 Phase 단위 지시 | — (지휘만) |
| `architect` | `architect.md` | 요구사항 분석, PLAN.md·DOMAIN.md 작성, Phase 설계 | `temp/` |
| `tdd-cycle-checker` | `tdd-cycle-checker.md` | Phase 단위 에이전트 순서 조율 및 결과 검증 | `temp/phase{N}_progress.md` |
| `unit-test` | `unit-test.md` | gmock으로 의존성 주입한 단위 테스트 작성 (신규 파일만) | `*Test.cpp` (신규) |
| `feature` | `feature.md` | 실패 테스트를 통과시키는 최소 프로덕션 코드 구현 | `*.cpp`, `*.h` |
| `refactoring` | `refactoring.md` | Phase 1(함수 현대화)·Phase 2(타입 안전성) 개선 | `*.cpp`, `*.h` |
| `clean-code` | `clean-code.md` | Phase 3 클래스 분리, SOLID 원칙 적용 | `*.cpp`, `*.h` |
| `tester` | `tester.md` | GTest 빌드·실행, PASS/FAIL 분류 보고 | — (실행만) |
| `coverage` | `coverage.md` | OpenCppCoverage 측정, 90% 미만 시 커밋 BLOCK | `temp/COVERAGE.md` |
| `git-manager` | `git-manager.md` | TDD 단계별 커밋·푸시 (사용자 confirm 후 실행) | — (git 명령만) |
| `docs-writer` | `docs-writer.md` | TEST_LIST.md, FEATURE_LIST.md, CHANGELOG.md 동기화 | `docs/*.md`, `README.md` |

### 에이전트 파일 수정 권한 요약

| 파일 유형 | 수정 가능 에이전트 |
|----------|-----------------|
| 프로덕션 코드 (`*.cpp`, `*.h`) | `feature`, `refactoring`, `clean-code` |
| 테스트 파일 (`*Test.cpp`) | `unit-test` (신규 생성만) |
| 문서 (`docs/*.md`, `README.md`) | `docs-writer` |
| `temp/` | `architect`, `tdd-cycle-checker` |
| git 명령 | `git-manager` |
| Bash 빌드 | `tester`, `coverage`, `feature`, `refactoring`, `clean-code` |

### Phase 표준 구조

| Phase | 내용 | 담당 에이전트 |
|-------|------|-------------|
| Phase 1 | 함수 내부 현대화 (busy-wait 제거, C→C++ 표준 함수) | `refactoring` |
| Phase 2 | 타입 안전성 개선 (enum class, 전역 변수 → 구조체) | `refactoring` |
| Phase 3 | 클래스 분리 / SOLID 적용 (SRP 기준 책임 분리) | `clean-code` |
| Phase 4 | 단위 테스트 및 커버리지 (gmock DI, 80%+ 목표) | `unit-test` + `coverage` |

### 커밋 타입

| 타입 | 단계 |
|------|------|
| `[docs]` | 문서·계획 변경 |
| `[unitTest]` | 테스트 작성 (RED) |
| `[feat]` | 기능 구현 (GREEN) |
| `[fix]` | 버그 수정 |
| `[refactor]` | 구조 개선 (REFACTOR) |

### 핵심 불변 원칙

- **이미 커밋된 테스트는 수정 불가** — Director 승인 없이 `*Test.cpp` 편집 금지
- **`git commit`은 사용자 confirm 후에만 실행** (`git-manager` 단독 결정 금지)
- **coverage 90% 미만 시 커밋 BLOCK** — 추가 테스트 작성 후 재측정

## PoC 항목

| PoC | Repository |
|-----|-----------|
| MVC 스켈레톤 코드 | https://github.com/jiyun7-Lee/ConsoleMVC-jiyun7Lee |
| 데이터 영속성 처리 (CRUD 포함) | https://github.com/jiyun7-Lee/DataPersistence-jiyun7Lee |
| 데이터 모니터링 Tool | https://github.com/jiyun7-Lee/DataMonitor-jiyun7Lee |
| Dummy 데이터 생성 Tool | https://github.com/jiyun7-Lee/DummyDataGenerator-jiyun7Lee |
| **메인 프로젝트** | `SampleOrderSystem-jiyun7Lee` |

## 빌드 환경

- **IDE**: Visual Studio 2022 (v145 toolset)
- **빌드 시스템**: MSBuild (`.vcxproj` / `.slnx`) — CMake 미사용
- **C++ 표준**: C++20 (`stdcpp20`)
- **인코딩**: `/utf-8` 컴파일 옵션
- **테스트 프레임워크**: Google Test + Google Mock (NuGet `gmock.1.11.0`)

### 프로젝트 구성

| 프로젝트 | 파일 | 역할 |
|---------|------|------|
| `Semicon` | `Semicon.vcxproj` | 메인 애플리케이션 |
| `SemiconTest` | `SemiconTest.vcxproj` | 단위 테스트 (gmock) |

### 빌드 명령 (MSBuild)

```
# 메인 앱 빌드
msbuild Semicon\Semicon.vcxproj /p:Configuration=Debug /p:Platform=x64

# 테스트 빌드 및 실행
msbuild Semicon\SemiconTest.vcxproj /p:Configuration=Debug /p:Platform=x64
x64\Debug\SemiconTest.exe
```

---

## 콘솔 한글 인코딩

Windows 콘솔에서 한글이 깨지지 않도록 반드시 아래 설정을 적용한다.

### C++ (main 진입점)

```cpp
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);   // 출력 인코딩 UTF-8
    SetConsoleCP(CP_UTF8);         // 입력 인코딩 UTF-8
    // ...
}
```

### 소스 파일 인코딩

- 모든 `.cpp`, `.h` 파일은 **UTF-8 (BOM 없음)** 으로 저장
- MSVC 사용 시 컴파일 옵션에 `/utf-8` 추가

```cmake
# CMakeLists.txt
if(MSVC)
    add_compile_options(/utf-8)
endif()
```

### 확인 방법

```
chcp        → 65001 이면 UTF-8 정상
```

---

## 과제 유의사항

- 사용 모델: **Sonnet / Effort: Medium** (Opus 사용 시 부정행위)
- 과제 종료 후 Claude Code `/logout` 실행
