# PRD — FR-01~03. 시료 관리

> 상위 문서: [PRD.md](../../PRD.md) | 관련 Phase: [Phase2_시료관리.md](../../temp/Phase2_시료관리.md)

---

## FR-01 시료 등록

### 입력

| 필드 | 타입 | 제약 |
|------|------|------|
| Sample ID | String | 중복 불가, 공백 불가 |
| Name | String | 공백 불가 |
| Average Production Time | double | 0 초과 (단위: 초/ea) |
| Yield Rate | double | 0.0 초과 ~ 1.0 이하 |

### 검증 규칙

- Sample ID 중복 → 등록 거부 (예외 발생)
- 이름 공백·빈 문자열 → 등록 거부
- 생산시간 0 이하 → 등록 거부
- 수율 범위 초과 (≤ 0 또는 > 1.0) → 등록 거부

### 결과

- 검증 통과 시 시료 저장 (Repository에 persist)

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(SampleServiceTest, RegisterSample_ValidInput_ShouldSave)

// Abnormal
TEST(SampleServiceTest, RegisterSample_DuplicateId_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_EmptyName_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_ZeroProductionTime_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_YieldOutOfRange_ShouldThrow)
TEST(SampleServiceTest, RegisterSample_NegativeYield_ShouldThrow)
```

---

## FR-02 시료 조회

### 기능

- 등록된 전체 시료 목록 출력

### 출력 항목

| 컬럼 | 필드 |
|------|------|
| Sample ID | sampleId |
| Name | name |
| Yield Rate | yieldRate |
| Average Production Time | averageProductionTime |
| Current Stock | currentStock (Inventory) |

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(SampleServiceTest, GetAllSamples_WithData_ShouldReturnList)

// Abnormal
TEST(SampleServiceTest, GetAllSamples_EmptyRepo_ShouldReturnEmptyList)
```

---

## FR-03 시료 검색

### 기능

- 시료 이름으로 부분 검색
- 일치하는 시료 목록 출력 (없으면 빈 목록)

### 입력

| 필드 | 설명 |
|------|------|
| keyword | 검색할 이름 문자열 |

### 관련 테스트 케이스 (gmock)

```cpp
// Normal
TEST(SampleServiceTest, SearchSample_MatchingKeyword_ShouldReturnResults)

// Abnormal
TEST(SampleServiceTest, SearchSample_NoMatch_ShouldReturnEmptyList)
TEST(SampleServiceTest, SearchSample_EmptyKeyword_ShouldReturnAllOrThrow)
```

---

## 연관 컴포넌트

| 레이어 | 클래스 |
|--------|--------|
| Service | `SampleService` |
| Controller | `SampleController` |
| View | `SampleView` |
| Repository (인터페이스) | `ISampleRepository` |
| Repository (구현체) | `InMemorySampleRepository`, `SampleJsonRepository` |
