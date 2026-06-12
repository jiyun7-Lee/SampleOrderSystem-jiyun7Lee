# S-Semi 반도체 시료 생산주문관리 시스템

가상의 반도체 회사 **S-Semi**의 시료(Sample) 생산주문관리 콘솔 애플리케이션.  
연구소, 팹리스(Fabless) 업체, 대학 연구실 등에 납품하는 반도체 시료의 주문·생산·출고를 통합 관리한다.

---

## 주요 기능

| 메뉴 | 기능 |
|------|------|
| [1] 시료 관리 | 시료 등록 / 목록 조회 / 이름 검색 |
| [2] 시료 주문 | 고객 주문 접수 (RESERVED 생성) |
| [3] 주문 승인/거절 | RESERVED 주문 목록 확인 후 승인·거절 처리 |
| [4] 모니터링 | 상태별 주문 수 및 시료별 재고 현황 |
| [5] 생산라인 조회 | 현재 생산 중 시료 및 대기 큐 확인 |
| [6] 출고 처리 | CONFIRMED 주문에 대해 출고 실행 |

---

## 주문 상태 흐름

```
RESERVED ──승인(재고 충분)──▶ CONFIRMED ──▶ RELEASE
         ──승인(재고 부족)──▶ PRODUCING ──생산완료──▶ CONFIRMED ──▶ RELEASE
         ──거절──▶ REJECTED
```

---

## 기술 스택

| 항목 | 내용 |
|------|------|
| 언어 | C++17 |
| 아키텍처 | MVC + Service Layer + Repository Pattern |
| 빌드 | Visual Studio 2022 (MSVC v145) / CMake |
| 테스트 | Google Test + Google Mock (gmock) |
| 데이터 영속성 | JSON 파일 (nlohmann/json) |
| 커버리지 | OpenCppCoverage (목표 80% 이상) |

---

## 빌드 및 실행

### 요구 사항

- Windows 10/11
- Visual Studio 2022 (MSVC v145)
- CMake 3.20 이상

### Visual Studio에서 빌드

1. `Semicon.vcxproj` 열기
2. 구성: **Release | x64** 선택
3. 빌드 (`Ctrl+Shift+B`)
4. 출력 경로: `x64/Release/Semicon.exe`

### 콘솔에서 빌드 (MSBuild)

```powershell
# temp/build_release.ps1 실행
.\temp\build_release.ps1
```

---

## 프로젝트 구조

```
Semicon/
├── src/
│   ├── main.cpp
│   ├── controller/          # MVC - Controller
│   ├── service/             # 비즈니스 로직
│   ├── model/
│   │   ├── domain/          # Sample, Order, Inventory, ProductionJob
│   │   └── repository/      # Repository 인터페이스
│   ├── view/                # MVC - View (콘솔 UI)
│   ├── persistence/         # JSON Repository 구현체
│   └── util/                # ITimeProvider, SystemTimeProvider
├── tests/                   # GTest 단위 테스트
│   ├── domain/
│   ├── service/
│   ├── repository/
│   └── controller/
├── temp/                    # 개발 계획 문서
│   ├── PLAN.md
│   ├── Phase1_기반구조_도메인모델.md
│   ├── Phase2_시료관리.md
│   ├── Phase3_주문관리.md
│   ├── Phase4_생산라인관리.md
│   ├── Phase5_모니터링_출고처리.md
│   ├── Phase6_데이터영속성.md
│   └── Phase7_단위테스트_커버리지.md
├── docs/
│   ├── TEST_LIST.md
│   ├── FEATURE_LIST.md
│   └── CHANGELOG.md
├── CMakeLists.txt
├── CLAUDE.md
├── PRD.md
└── README.md
```

---

## 도메인 모델

### Sample (시료)

| 필드 | 타입 | 설명 |
|------|------|------|
| sampleId | String | 고유 식별자 (예: S-001) |
| name | String | 시료명 |
| averageProductionTime | double | 평균 생산시간 (초/ea) |
| yieldRate | double | 수율 (0.0 초과 ~ 1.0 이하) |

### Order (주문)

| 필드 | 타입 | 설명 |
|------|------|------|
| orderId | String | 주문번호 (예: ORD-20260416-0001) |
| sampleId | String | 참조 시료 ID |
| customerName | String | 고객명 |
| quantity | int | 주문 수량 |
| status | OrderStatus | 주문 상태 |
| createdAt | DateTime | 주문 생성 시각 |

### 생산량 계산 공식

```
실 생산량 = ceil(부족분 / (수율 * 0.9))
총 생산 시간 = 평균 생산시간(초) * 실 생산량
```

---

## 개발 방법론

- **TDD** (Test-Driven Development): RED → GREEN → REFACTOR
- **SOLID 원칙** 준수
- **gmock** 의존성 주입 기반 단위 테스트
- **Agentic Engineering**: `.claude/agents/` 11개 전문 에이전트 자동화

---

## 참고 링크

| 항목 | 링크 |
|------|------|
| 메인 프로젝트 | [SampleOrderSystem-jiyun7Lee](https://github.com/jiyun7-Lee/SampleOrderSystem-jiyun7Lee) |
| MVC PoC | [ConsoleMVC-jiyun7Lee](https://github.com/jiyun7-Lee/ConsoleMVC-jiyun7Lee) |
| 데이터 영속성 PoC | [DataPersistence-jiyun7Lee](https://github.com/jiyun7-Lee/DataPersistence-jiyun7Lee) |
| 모니터링 PoC | [DataMonitor-jiyun7Lee](https://github.com/jiyun7-Lee/DataMonitor-jiyun7Lee) |
| Dummy 데이터 PoC | [DummyDataGenerator-jiyun7Lee](https://github.com/jiyun7-Lee/DummyDataGenerator-jiyun7Lee) |
