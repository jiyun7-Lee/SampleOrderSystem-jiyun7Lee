# PRD — NFR. 비기능 요구사항 및 설계 원칙

> 상위 문서: [PRD.md](../../PRD.md)

---

## 7. 비기능 요구사항

### 아키텍처

```
Model
 ├─ domain     (Sample, Order, Inventory, ProductionJob)
 └─ repository (인터페이스 + 구현체)

View
 └─ Console UI

Controller
 └─ 사용자 입력 처리 → Service 호출

Service
 └─ 비즈니스 로직 처리
```

### 디렉토리 구조

```
src
 ├─ controller
 ├─ service
 ├─ model
 │   ├─ domain
 │   └─ repository
 ├─ view
 ├─ persistence
 ├─ util
 └─ main

tests
 ├─ controller
 ├─ service
 ├─ repository
 └─ domain
```

### 데이터 영속성

- 저장 대상: Sample, Order, Inventory, Production Queue
- 방식: JSON 파일 (nlohmann/json)
- 애플리케이션 시작 시 데이터 로드 / 변경 시 자동 저장

### 예외 처리

- 중복 Sample ID
- 존재하지 않는 Sample / Order ID
- 잘못된 입력값 (빈 문자열, 음수 수량, 범위 밖 수율)
- 잘못된 상태 전이

### 테스트

- Domain / Service / Repository 단위 테스트
- 코드 커버리지 **80% 이상**
- **Google Mock(gmock)** 으로 의존성 주입
  - Repository, TimeProvider 등 외부 의존성은 모두 mock 객체로 대체
  - 서비스 테스트 시 실제 저장소·시스템 시간에 의존하지 않음

---

## 8. 시간 처리 설계

### 운영 환경

```cpp
SystemTimeProvider  // 실제 시간 사용
```

### 테스트 환경

```cpp
ITimeProvider       // 인터페이스를 통해 시간 조회 (DI 적용)
MockTimeProvider    // 단위 테스트 시 gmock으로 주입
FakeTimeProvider    // 시간 경과 시뮬레이션

// 예시 — 300초 경과 시뮬레이션
mockTime.Advance(std::chrono::seconds(300));
```

> 모든 서비스는 `std::chrono::system_clock::now()`를 **직접 호출하지 않는다**.  
> 실제 300초를 기다리는 테스트는 허용하지 않는다.

### gmock 기반 단위 테스트 규칙

모든 서비스 테스트는 **Google Mock(gmock)** 으로 의존성을 주입하여 작성한다.

```cpp
class MockTimeProvider : public ITimeProvider {
public:
    MOCK_METHOD(std::chrono::system_clock::time_point, Now, (), (override));
};

class MockOrderRepository : public IOrderRepository {
public:
    MOCK_METHOD(void, Save, (const Order&), (override));
    MOCK_METHOD(std::optional<Order>, FindById, (const std::string&), (const, override));
};

TEST(OrderServiceTest, ApproveOrder_SufficientStock_ShouldConfirm) {
    MockOrderRepository mockOrderRepo;
    MockInventoryRepository mockInventoryRepo;
    MockTimeProvider mockTime;

    EXPECT_CALL(mockInventoryRepo, FindBySampleId(_))
        .WillOnce(Return(Inventory{"S-001", 100, 0}));
    EXPECT_CALL(mockOrderRepo, Save(_)).Times(1);

    OrderService service(mockOrderRepo, mockInventoryRepo, mockTime);
    service.ApproveOrder("ORD-001");
}
```

---

## 9. 설계 원칙

- MVC Architecture
- SOLID 원칙 준수
- DI(의존성 주입) 적극 활용
- Testability 최우선
- Service Layer 중심 설계
- Repository와 Service 완전 분리
- Mock 기반 Unit Test
- 생산라인 상태를 시간 기반으로 시뮬레이션 가능

---

## 10. 완료 기준 (Definition of Done)

- [ ] 모든 기능 요구사항(FR-01 ~ FR-11) 구현 완료
- [ ] 데이터 영속성 동작 확인 (재시작 후 데이터 유지)
- [ ] 단위 테스트 작성 완료 (커버리지 80% 이상)
- [ ] 테스트 전체 통과
- [ ] Clean Code 원칙 준수
- [ ] Commit History 관리 완료
- [ ] README.md 작성 완료
- [ ] CLAUDE.md 문서 최신화
