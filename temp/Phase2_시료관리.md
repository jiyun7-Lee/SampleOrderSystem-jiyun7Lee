# Phase 2 — 시료 관리 기능 (FR-01~03)

## 목표

시료(Sample) 등록·조회·검색 기능을 MVC 구조로 구현한다.  
메인 메뉴 **[1] 시료 관리** 서브메뉴가 완전히 동작하는 상태를 목표로 한다.

## 담당 에이전트

| 단계 | 에이전트 |
|------|---------|
| 테스트 작성 | `unit-test` |
| 구현 | `feature` |
| SOLID 검토 | `clean-code` |
| 빌드·실행 | `tester` |
| 커밋 | `git-manager` |

---

## Step 2-1: SampleService 구현

### 기능

| 메서드 | 설명 | FR |
|--------|------|-----|
| `RegisterSample(Sample)` | 신규 시료 등록 | FR-01 |
| `GetAllSamples()` | 전체 시료 목록 조회 | FR-02 |
| `SearchByName(name)` | 이름으로 시료 검색 | FR-03 |

### 검증 규칙 (FR-01)

- Sample ID 중복 → 예외 처리
- 이름 공백 → 예외 처리
- 생산시간 0 이하 → 예외 처리
- 수율 0 이하 또는 1 초과 → 예외 처리

### 단위 테스트 케이스 (gmock)

```cpp
// tests/service/SampleServiceTest.cpp
class MockSampleRepository : public ISampleRepository {
public:
    MOCK_METHOD(void, Save, (const Sample&), (override));
    MOCK_METHOD(std::optional<Sample>, FindById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindAll, (), (const, override));
    MOCK_METHOD(std::vector<Sample>, FindByName, (const std::string&), (const, override));
    MOCK_METHOD(bool, ExistsById, (const std::string&), (const, override));
};

TEST(SampleServiceTest, RegisterSample_DuplicateId_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_EmptyName_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_ZeroProductionTime_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_InvalidYieldRate_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_ValidInput_ShouldSave)
TEST(SampleServiceTest, GetAllSamples_ShouldReturnAll)
TEST(SampleServiceTest, SearchByName_ShouldReturnMatching)
```

### 완료 기준

- [ ] SampleService 구현 완료
- [ ] 모든 검증 로직 동작
- [ ] 단위 테스트 PASS

---

## Step 2-2: SampleController 구현

### 역할

사용자 입력을 받아 SampleService를 호출하고 결과를 SampleView에 전달한다.

```cpp
class SampleController {
public:
    SampleController(SampleService& service, SampleView& view);
    void Run();           // 시료 관리 서브메뉴 루프
    void HandleRegister();
    void HandleList();
    void HandleSearch();
};
```

### 완료 기준

- [ ] SampleController 구현 완료
- [ ] 서브메뉴 [1] 등록 / [2] 목록 / [3] 검색 / [0] 뒤로 동작

---

## Step 2-3: SampleView 구현

### 출력 항목 (FR-02)

| 컬럼 | 내용 |
|------|------|
| ID | Sample ID |
| 시료명 | Name |
| 평균 생산시간 | averageProductionTime (초/ea) |
| 수율 | yieldRate |
| 현재 재고 | currentStock |

### 시료 관리 서브메뉴 UI

```
==============================
[1] 시료 관리
==============================
[1] 시료 등록  [2] 시료 목록  [3] 시료 검색  [0] 뒤로
선택 > _
```

### 완료 기준

- [ ] 시료 등록 입력 화면
- [ ] 시료 목록 테이블 출력
- [ ] 시료 검색 결과 출력
- [ ] 한글 깨짐 없음

---

## Step 2-4: 메인 메뉴 [1] 시료 관리 연결

메인 메뉴에서 `[1]` 선택 시 SampleController로 진입한다.

### 메인 메뉴 현황판 표시 항목

```
시스템 현황  2026-06-12 09:32:15
등록 시료  N종    총 재고  N ea
전체 주문  N건    생산라인  N건 대기
```

### 완료 기준

- [ ] 메인 메뉴 → [1] → SampleController 진입
- [ ] 현황판 정보 정상 출력

---

## Phase 2 완료 기준

- [ ] FR-01 시료 등록 동작 확인
- [ ] FR-02 시료 조회 (재고 포함) 동작 확인
- [ ] FR-03 시료 검색 동작 확인
- [ ] 단위 테스트 PASS
- [ ] 커버리지 측정 완료
- [ ] `[feat] Phase 2 — 시료 관리 기능` 커밋 완료
