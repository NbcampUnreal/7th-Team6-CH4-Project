--------------------------------------------------------------------------------
                           CH4 6조 팀 프로젝트 피드백
--------------------------------------------------------------------------------

안녕하세요. 6조 여러분. 김봉재 튜터입니다.

CH4 들어와서 멀티플레이 게임 제작을 처음 접하면서, 많은 부분이 어려워 하셨지만
팀 프로젝트를 잘 해내신 모습을 보니 매우 흐뭇 합니다.

현재는 다들 부족하다고 느끼실 테지만, 지금 처럼 초심을 잃지 않고 재미있게
개발을 진행 하다보면 어느땐가 많이 편해 지시는 날이 오실겁니다.

처음으로 도전해본 프로젝트에서 얻어가시는 것이 많았으면 하는 마음에,
실제로 실무에서 개발시 고려 해야할 부분들에 대한 내용들을 담았습니다.

실제로 리팩토링을 진행한 샘플 코드는 팀 프로젝트에 커밋, 푸시 후 PR 해 놓았으니
각자 확인 하시면 좋겠습니다.

현재 작성하신 프로젝트의 코드 구조, 사용법, 폴더 구조, 클래스명 선정에서
일관성과 명확성이 떨어지는 지점을 카테고리별로 모아, 각 항목에 대해 어떻게
고치면 좋은지 가이드와 함께 코드도 준비해 보았습니다.

모든 부분에 대해 다 적용을 하시는것 보다는, 실무에서 이런식으로 구조를 잡고
코드는 이런식으로 작성을 한다 정도의 느낌으로, 소설책을 읽는 느낌으로
가볍게 진행 하시면 도움이 되실것 같습니다.

CH4 팀 프로젝트 고생 많으셨습니다.

--------------------------------------------------------------------------------
                                    목차
--------------------------------------------------------------------------------

   1. 폴더 구조
   2. 클래스 명명 규칙
   3. 책임 분리와 단일 책임 원칙
   4. 컴포넌트 기반 설계
   5. 반복되는 바인딩 코드를 매크로로 정리
   6. 도메인 트리 분리: 장비 시스템과 스킬 시스템
   7. 엔진 베이스 클래스 선택
   8. UPROPERTY 와 UFUNCTION 사용법
   9. 포인터/참조 타입 표기
  10. 멀티플레이 패턴 (서버 권한, RPC, 복제)
  11. 데이터 자산화와 매직 넘버 제거
  12. Include 와 의존성 관리
  13. 코드 청결성 (안 쓰는 코드, 중복, 주석)
  14. 도메인 격리와 캐스트 타입 좁히기
  15. 리팩토링 작업 방식 가이드


--------------------------------------------------------------------------------
                                1. 폴더 구조
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
1.1 폴더명에 공백 금지
--------------------------------------------------------------------------------

폴더명에 공백이 들어가면 빌드 시스템, 쉘, IDE, 일부 툴체인에서 처리가
까다롭고 (따옴표로 감싸야 하거나 이스케이프 처리가 필요해서), 같은 모듈
내에서 일부 폴더만 공백을 쓰면 일관성이 깨집니다.

문제:
    Source/CH4_TeamProject/
    +-- Title and Lobby/                        (공백)
    +-- Item/Equippable/Meel Weapon/            (공백 + 오타)
    +-- Item/Equippable/Ranged Weapon/          (공백)

가이드:
    - 폴더명은 PascalCase (단어마다 첫 글자 대문자) 로 한 단어 또는 단어 연결로 작성합니다.
    - 영문만 사용합니다. 공백/하이픈/언더스코어는 쓰지 않습니다.
    - 오타가 없도록 합니다 (Meel -> Melee, Thorwble -> Throwable, Ojective -> Objective).

올바른 예:
    Source/CH4_TeamProject/
    +-- TitleAndLobby/
    +-- Weapon/Melee/
    +-- Weapon/Ranged/
    +-- Item/Throwable/


--------------------------------------------------------------------------------
1.2 폴더는 각자의 역할을 잘 나타내야 합니다
--------------------------------------------------------------------------------

폴더는 단순한 분류함이 아니라, 그 안의 코드가 어떤 책임을 다루는지 보여주는
단위입니다. 한 가지 기능에 속하는 코드가 여러 폴더에 흩어지면, 클래스를 찾거나 수정할때
매우 힘들어 집니다.

문제:
    Source/CH4_TeamProject/
    +-- Game/                          (GameMode/State/PlayerState/Instance 가 섞임)
    +-- Game/GameManagers/             (FinalDefenceManager 가 GameMode 의 자식인데 별도 폴더)
    +-- Title and Lobby/               (GameMode 와 PlayerController 가 섞임)
    +-- Player/                        (Character/PC/AnimInstance/Component 섞임)
    +-- Item/Equippable/               (장비 시스템이 Item 폴더 안에 섞임)
    +-- UI/                            (위젯 베이스, HUD, 인벤토리 등 다 섞임)

가이드 (역할 기준 분리):
    Source/CH4_TeamProject/
    +-- Framework/                     (Unreal Engine 핵심 프레임워크 클래스)
    |   +-- GameMode/
    |   +-- GameState/
    |   +-- PlayerState/
    |   +-- GameInstance/
    +-- Player/
    |   +-- Character/
    |   +-- Controller/
    |   +-- Animation/
    |   +-- Component/
    +-- Monster/
    |   +-- Character/
    |   +-- AI/
    |   |   +-- Controller/
    |   |   +-- BTNode/
    |   +-- Animation/
    +-- Weapon/                        (장비 시스템: 별도 도메인)
    |   +-- Data/
    +-- Item/                          (스킬/소비 시스템: 장비와 분리)
    |   +-- Data/
    +-- SpawnPoint/
    |   +-- Data/
    +-- UI/
    +-- AnimNotify/
    +-- Common/                        (여러 도메인이 공용으로 쓰는 enum/struct/매크로)

원칙:
    - 같은 도메인의 다른 종류 (컴포넌트, 컨트롤러, 애니인스턴스) 는 하위
      폴더로 둡니다.
    - DataAsset 클래스는 자기 도메인 폴더 안의 Data/ 하위 폴더에 둡니다.
    - 기능적으로 다른 시스템 (장비 vs 스킬) 은 별도 최상위 폴더로 분리합니다.
      6장을 참고해 주세요.


--------------------------------------------------------------------------------
1.3 클래스 1개 = 파일 1개
--------------------------------------------------------------------------------

한 헤더 파일에 여러 UCLASS 가 들어 있으면 그 헤더가 길어지고, 그 헤더를
#include 하는 파일들이 자기에게 필요 없는 클래스까지 함께 가져오게 됩니다.
결과적으로 컴파일 시간이 늘고 의존 관계가 복잡해집니다.

가이드:
    - UCLASS 하나당 .h/.cpp 한 쌍으로 둡니다.
    - USTRUCT/Enum 도 도메인이 명확하면 별도 파일로 분리합니다 (DataAsset
      처럼 짝지어 다니는 USTRUCT 는 예외로 같은 파일에 둬도 됩니다).
    - 파일명은 클래스명에서 U/A/I prefix 만 떼고 그대로 사용합니다.


--------------------------------------------------------------------------------
                            2. 클래스 명명 규칙
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
2.1 모듈 단위의 Epic prefix 는 빠짐없이 작성
--------------------------------------------------------------------------------

- 엔진 클래스 (UCameraComponent, AGameMode 등) 와 이름이 겹치지 않을 수 있도록 해줍니다.
- 모듈이 추가 되었을때, 같은 기능을 하는 클래스를 필요로 하는 경우에도 분류가 쉽습니다.
- 우리 프로젝트 코드 전체에서 표기가 일관되게 유지됩니다.
- 코드 자동완성이나 검색에서 우리 코드만 골라 보기 쉽습니다 (CH4 로 시작하는 것만 필터링 가능).

문제:
    class ABaseItem;             (CH4 prefix 없음)
    class AEquippable;           (CH4 prefix 없음)
    class UWeaponData;           (CH4 prefix 없음)
    class AZombieBase;           (CH4 prefix 없음)
    class UMyCameraShakeBase;    (My 라는 의미 없는 prefix)

가이드: 모든 신규/리팩토링 클래스에 CH4 prefix 를 적용합니다.

    class ACH4ItemBase;
    class ACH4EquippableItem;
    class UCH4WeaponData;
    class ACH4ZombieBase;
    class UCH4CameraShake;


--------------------------------------------------------------------------------
2.2 Base suffix 는 최상위 추상 클래스에서 사용
--------------------------------------------------------------------------------

상속 트리에서 Base 단어가 두 군데 이상 쌓이거나 prefix 와 suffix 가 섞이면
어떤 클래스가 트리의 어디에 있는지 알기 어렵습니다.

문제:

    class ABaseItem;             (Base 가 prefix 형식)
    class AZombieBase;           (Base 가 suffix 형식 - 형식이 다름)
    class AEquippableBase;       (ABaseItem 자식인데 또 Base 가 붙음)

가이드:
    - 한 도메인 트리에서 가장 위에 있는 추상 클래스 한 개에만 Base 를 suffix 로 붙입니다.
    - 부모가 이미 Base 인 자식은 자기가 추상이라도 Base 를 붙이지 않습니다.
    - 자식 클래스가 전혀 없는 클래스에는 Base 를 붙이지 않습니다.

올바른 예 (Item 도메인):

    ACH4ItemBase                    (트리 최상위 추상, Base 유지)
    +-- ACH4AmmoItem
    +-- ACH4HealItem
    +-- ACH4ThrowableItem

올바른 예 (Weapon 도메인 - Item 과 별개 트리):

    ACH4WeaponBase                  (트리 최상위 추상, Base 유지)
    +-- ACH4MeleeWeapon
    +-- ACH4RangedWeapon


--------------------------------------------------------------------------------
2.3 클래스명은 단수형으로 작성
--------------------------------------------------------------------------------

클래스 1 개는 인스턴스 한 종류를 의미합니다. 복수형으로 클래스명을 지으면 한
인스턴스가 여러 개 같은 느낌을 주고, 형제 클래스가 단수형일 때 형식이
어긋납니다. 복수형은 보통 TArray 등 컨테이너에서 사용합니다.

문제:
    class AMeleeWeapons;       (복수)
    class ARangedWeapons;      (복수)
    class AThorwbleItems;      (복수 + 오타)
    class AHealItem;           (단수)

가이드:
    class ACH4MeleeWeapon;
    class ACH4RangedWeapon;
    class ACH4ThrowableItem;
    class ACH4HealItem;


--------------------------------------------------------------------------------
2.4 클래스명은 명확하게 의미가 전달 되도록 작성
--------------------------------------------------------------------------------

클래스명만 보고 그 클래스가 무엇을 상속한 자식인지 (GameMode 자식인지, AnimNotify 자식인지)
짐작할 수 있어야 합니다. 약어나 모호한 어미를 쓰면 클래스가 어떤 종류인지 알기 어렵습니다.

- GameMode 의 자식 클래스는 이름 끝에 GameMode 를 붙입니다.
- AnimNotify 의 자식 클래스는 이름 끝에 Notify 를 붙입니다.
- "Manager" 같이 무엇을 관리하는지 알 수 없는 어미는 쓰지 않습니다.
  무엇을 관리하는지 구체적으로 적어 주세요.

문제:
    class ACH4Title : public ACH4GameMode;
    (이름은 위젯이나 데이터처럼 보이는데 실제로는 GameMode 의 자식입니다)

    class AFinalDefenceManager : public ACH4GameMode;
    (Manager 라는 어미가 붙어서 GameMode 자식이라는 사실이 가려집니다)

    class UMeleeWeaponNF : public UAnimNotify;
    (NF 가 무슨 약어인지 알 수 없습니다)

가이드:
    class ACH4TitleGameMode           : public AGameModeBase;
    class ACH4DefenseGameMode         : public AGameModeBase;
    class UCH4MeleeWeaponAttackNotify : public UAnimNotify;


--------------------------------------------------------------------------------
2.5 영국식 / 미국식 표기 통일
--------------------------------------------------------------------------------

같은 단어가 두 표기로 섞여 있으면 코드 검색이 불완전해집니다. 예를 들어
"Defense" 로 전체 검색을 해도 "Defence" 로 적힌 곳은 결과에서 빠집니다.
식별자 이름 변경이나 일괄 정리가 필요할 때 빠진 부분을 놓치기 쉽습니다.
지형 특성상 한국은 미국식으로 사용하는 것이 보편적 입니다.

문제: Defence (영국식) 와 Defense (미국식) 혼재.
    class AFinalDefenceManager;            (영국식)
    float FinalDefenceWaveTimer;
    void StartFinalDefenceTimer();
    EGamePhase::FinalDefense               (enum 만 미국식)

가이드: 본 프로젝트는 미국식 Defense 로 통일합니다.


--------------------------------------------------------------------------------
2.6 식별자 오타는 즉시 정정
--------------------------------------------------------------------------------

문제:
    float mouseSpeed;            (camelCase 위반 + 의미 모호)
    ACH4GameState* GamsState;    (오타: GameState)
    bool bUSingGrenade;          (오타: bIsUsingGrenade)
    UMeelWeaponData;             (오타: Meel -> Melee)
    int32 MinHeelItem;           (오타: Heel -> Heal)

가이드:
    float MouseSensitivity;
    ACH4GameState* GameState;
    bool bIsUsingGrenade;
    UCH4MeleeWeaponData;
    int32 MinHealItem;

오타를 정정할 때는 그 식별자를 BP 가 참조하는지 먼저 확인한 후, 다른 변경과
섞지 않고 따로 진행해 주세요 (오타 정정 변경에 다른 변경이 섞이면 리뷰가
어려워지고 BP 가 깨지는 원인을 찾기 힘들어집니다).


--------------------------------------------------------------------------------
2.7 bool 변수는 b prefix + 긍정형
--------------------------------------------------------------------------------

문제:
    bool IsSprinting;            (b 누락)
    bool bDisableInput;          (부정형: 비활성화 상태일 때 true)

가이드:
    bool bIsSprinting;           (b prefix)
    bool bInputEnabled;          (긍정형: 활성화 상태일 때 true)

부정형 bool 은 호출하는 쪽에서 if (!bDisableInput) 같은 이중 부정 ("비활성화
가 아니면" = 활성화) 이 되어 읽기 어렵습니다.


--------------------------------------------------------------------------------
                  3. 책임 분리와 단일 책임 원칙
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
3.1 거대 클래스 분리: 책임별 컴포넌트로 쪼개기
--------------------------------------------------------------------------------

원본 ACH4Character 는 헤더 385 줄, cpp 1187 줄에 책임 10 개를 떠안고 있습니다.
이런 클래스는 한 줄만 수정해도 어떤 기능이 같이 영향을 받는지 예측하기 어렵습니다.

원본 한 클래스에 들어있던 책임:

    1. 메시/스켈레탈 메시 초기화
    2. 카메라 (SpringArm, Camera, 정조준, 프리룩)
    3. 입력 매핑 및 바인딩
    4. 이동/스프린트
    5. 체력/피격/다운
    6. 인벤토리 (회복 아이템, 수류탄)
    7. 무기 장착/발사/재장전
    8. 상호작용 (아이템 줍기, 동료 부활)
    9. 애니메이션 재생 (피격, 줍기, 다운, 사망, 부활, 발사, 재장전 몽타주)
    10. 위 모든 것의 RPC 20 개 이상

가이드:
책임을 UActorComponent 로 쪼개고,
Character 본체는 컴포넌트 생성/연결 + 입력 라우팅 + 공통 애니메이션 멀티캐스트만 담당하게 합니다.

본 프로젝트 ACH4PlayerCharacter 의 컴포넌트 분담:

    UCH4CameraComponent      - 3 인칭 카메라, 정조준 시각, 프리룩.
                               SpringArm/Camera 참조 + 정조준 보간 + 회전 모드 갱신.

    UCH4MovementComponent    - 보행/스프린트/정조준 이동.
                               UCharacterMovementComponent 확장. 4.3 참고.

    UCH4HealthComponent      - 체력/피격/다운/사망/부활.
                               TakeDamage 위임, OnRep_IsDowned, OnPlayerDowned/
                               OnHealthChanged 델리게이트.

    UCH4InteractionComponent - 상호작용 (아이템 줍기/동료 부활).
                               OnPickupRequested/OnReviveRequested 델리게이트.

    UCH4SkillComponent       - 액티브 스킬 (회복 사용/수류탄 투척).
                               HealItemCount/GrenadeCount 복제, Server_UseHealItem/
                               Server_ThrowGrenade RPC.

분담 후 ACH4PlayerCharacter 본체에 남는 책임:

    - 컴포넌트 생성과 연결 (생성자), BeginPlay 단계에서 컴포넌트 초기화 위임
    - IGenericTeamAgentInterface 구현
    - SetupPlayerInputComponent 에서 입력 액션을 적절한 컴포넌트로 전달
    - 애니메이션 인스턴스 보유 + 공통 액션 멀티캐스트 (Multicast_PlayAction)
    - 무기 슬롯 (4 슬롯 고정 인벤토리)

샘플 코드 (Character 본체 - 컴포넌트 보유):

    class ACH4PlayerCharacter : public ACharacter, public IGenericTeamAgentInterface
    {
        GENERATED_BODY()

    public:
        ACH4PlayerCharacter(const FObjectInitializer& ObjectInitializer);

        virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
        virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
                                 AController* EventInstigator, AActor* DamageCauser) override;

    public:
        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        TObjectPtr<UCH4CameraComponent> CameraLogicComponent;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        TObjectPtr<UCH4MovementComponent> CH4MovementComponent;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        TObjectPtr<UCH4HealthComponent> HealthComponent;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        TObjectPtr<UCH4InteractionComponent> InteractionComponent;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        TObjectPtr<UCH4SkillComponent> SkillComponent;
    };

샘플 코드 (Character 의 입력 라우팅 - 본체는 입력을 컴포넌트로 전달만):

    void ACH4PlayerCharacter::HandleInputAction(ECH4PlayerInputAction Action, bool bActive)
    {
        switch (Action)
        {
        case ECH4PlayerInputAction::Sprint:
            bActive ? CH4MovementComponent->StartSprint()
                    : CH4MovementComponent->StopSprint();
            break;
        case ECH4PlayerInputAction::Aim:
            bActive ? CameraLogicComponent->StartAim()
                    : CameraLogicComponent->StopAim();
            break;
        case ECH4PlayerInputAction::Interact:
            if (bActive) InteractionComponent->Interact();
            break;
        case ECH4PlayerInputAction::HealSkill:
            if (bActive) SkillComponent->OnApplyItemEffect();
            break;
        case ECH4PlayerInputAction::ThrowSkill:
            if (bActive) SkillComponent->OnThrowGrenade();
            break;
        // ...
        }
    }

분리 후 효과:
    - 각 컴포넌트는 자기 도메인 멤버/RPC 만 가집니다. 한 책임을 바꿔도 다른 책임이 깨지지 않습니다.
    - 같은 컴포넌트를 Monster 와 AIPawn 같은 다른 액터에도 붙여서 재 사용할 수있습니다.
    - 컴포넌트 사이의 결합도는 4 장의 델리게이트 패턴으로 낮게 유지합니다.


--------------------------------------------------------------------------------
3.2 Manager 라는 이름의 별도 클래스 만들지 않기
--------------------------------------------------------------------------------

게임 규칙, 판정, 타이머 같은 게임 흐름 제어는 GameMode 가 담당해야 합니다.
이름 끝에 Manager 를 붙인 별도 Actor 가 GameMode 와 같이 게임 흐름을
제어하면, 어떤 클래스가 최종 결정권을 갖는지 헷갈리고, GameMode 와 Manager
가 같은 상태 (타이머, 플레이어 카운트 등) 를 따로 들고 있다가 서로 어긋나는
일이 생깁니다.

문제:

    class AFinalDefenceManager : public ACH4GameMode
    {
        // GameMode 의 자식인데 'Manager' 라는 이름을 붙임.
        // 최종 방어 Wave Timer 만 따로 들고 있음.
        float FinalDefenceWaveTimer;
        void StartFinalDefenceTimer();
    };

가이드: 본래 GameMode 에 흡수합니다.

    class ACH4DefenseGameMode : public AGameModeBase
    {
        GENERATED_BODY()

    public:
        void StartDefenseTimer();
        void FinalDefenseWaveSpawn();

    private:
        float DefenseWaveTimer = 0.0f;
    };

Manager 가 정말로 별도 책임을 가지는 경우 (예: 객체 풀링) 는 USubsystem
또는 UActorComponent 로 만듭니다. Actor 클래스로 만든 Manager 는 대부분 잘못된
설계이며, 본래 그 책임을 가져야 할 GameMode/GameState/Subsystem 으로 옮겨야
합니다.


--------------------------------------------------------------------------------
3.3 GameInstance 에 게임플레이 상태 저장 금지
--------------------------------------------------------------------------------

UGameInstance 는 맵을 이동해도 살아남는 객체입니다. 여기에 게임플레이 상태를
두면, 새 매치를 시작해도 이전 매치의 점수/타이머/카운트가 그대로 남아있어서
잘못된 상태로 게임이 시작됩니다.

문제:

    class UCH4GameInstance : public UGameInstance
    {
        UPROPERTY(BlueprintReadWrite) int32 TotalScore;
        UPROPERTY(BlueprintReadWrite) int32 CurrentLevelIndex;
        UPROPERTY(BlueprintReadOnly)  float RemainingTime;
        void UpdateTimer();
    };

가이드: 상태 종류별로 저장 위치를 정합니다

    게임 규칙/판정 (서버에서만 결정)          -> GameMode
    모든 플레이어가 알아야 할 상태            -> GameState
    (Phase, Timer, AlivePlayerCount)
    플레이어별 데이터 (LifeState, Score)     -> PlayerState
    본인만 보는 UI/입력                      -> PlayerController
    세션을 넘어 영속되는 데이터                -> GameInstance 또는 SaveGame
    (옵션, 계정 정보)

GameState 로 이동 후의 모습:

    class ACH4DefenseGameState : public AGameStateBase
    {
        UPROPERTY(ReplicatedUsing = OnRep_ServerTime, BlueprintReadOnly, Category = "Time")
        float ServerTime = 0.f;

        UPROPERTY(ReplicatedUsing = OnRep_GamePhase, BlueprintReadOnly, Category = "Phase")
        EGamePhase GamePhase = EGamePhase::None;

        UPROPERTY(Replicated, BlueprintReadOnly, Category = "Players")
        int32 AlivePlayerCount = 0;
    };


--------------------------------------------------------------------------------
3.4 추상 부모 클래스에 특정 도메인 전용 코드를 두지 않기
--------------------------------------------------------------------------------

UCLASS(Abstract) 로 선언한 부모 클래스가 특정 모드 전용 코드를 들고 있으면,
그 부모를 상속받은 다른 모드의 자식 클래스도 자기와 무관한 멤버를 강제로
갖게 됩니다.

문제:

    UCLASS(Abstract)
    class ACH4PlayerController : public APlayerController
    {
    public:
        // 방어전 모드 전용
        UFUNCTION(Client, Reliable) void Client_PlayDownAnim();
        UFUNCTION(Client, Reliable) void Client_PlayReviveAnim();
        UFUNCTION(Client, Reliable) void Client_SetPlayerDownedUI(bool bShow);
        UFUNCTION(BlueprintCallable, Server, Reliable) void BackToLobby();
        void ShowGameClear(); void ShowGameOver(); void StartGame();

        // 모든 모드 공통
        void HideCurrentWidget();
    };

    // 타이틀 PlayerController 는 방어전 멤버까지 모두 떠안게 됩니다
    class ATitlePlayerController : public ACH4PlayerController { };

가이드:
해당 클래스 전용 코드는 분리합니다. 추상 부모를 직접 수정할 수 없는 단계에서는, 
클래스를 APlayerController 에서 직접 상속받는 독립 클래스로 작성 합니다.

본 프로젝트의 적용 예 (ACH4DefensePlayerController):

    UCLASS()
    class ACH4DefensePlayerController : public APlayerController, public IGenericTeamAgentInterface
    {
        GENERATED_BODY()

    public:
        ACH4DefensePlayerController();

        virtual FGenericTeamId GetGenericTeamId() const override;

        UFUNCTION(BlueprintCallable, Category = "GameFlow")
        void StartGame();

        UFUNCTION(BlueprintCallable, Category = "GameFlow")
        void ShowGameClear();

        UFUNCTION(BlueprintCallable, Server, Reliable, Category = "GameFlow")
        void BackToLobby();

        UFUNCTION(Client, Reliable) void Client_DisablePlayerInput();
        UFUNCTION(Client, Reliable) void Client_EnablePlayerInput();
        UFUNCTION(Client, Reliable) void Client_PlayDownAnim();
        UFUNCTION(Client, Reliable) void Client_PlayReviveAnim();
        UFUNCTION(Client, Reliable) void Client_InvokeGameClearUI();
        UFUNCTION(Client, Reliable) void Client_InvokeGameLoseUI();
        UFUNCTION(Client, Reliable) void Client_SetPlayerDownedUI(bool bShow);

        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
        TSubclassOf<UUserWidget> HUDWidgetClass;

        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
        TSubclassOf<UUserWidget> GameClearWidgetClass;

        UPROPERTY()
        TObjectPtr<ACH4PlayerCharacter> ControlledCharacter;
    };

이때 방어전과 무관한 멤버 (ShowStartMenu, ShowGameRule, StartMenuClass,
GameRulesWidgetClass, HUDLobbyWidgetClass) 는 제외 합니다. 이렇게 하면
방어전 PlayerController 가 로비/타이틀 도메인 코드를 우연히 호출할 일이
없어, 섞이는 것을 코드 단계에서 막을 수 있습니다.


--------------------------------------------------------------------------------
                            4. 컴포넌트 기반 설계
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
4.1 컴포넌트는 자기 도메인 상태와 RPC 만 보유
--------------------------------------------------------------------------------

3.1 의 책임 분리를 실제로 적용할 때, 각 컴포넌트는 자기가 담당하는 도메인의 상태와
RPC 를 보유합니다.
외부 클래스가 컴포넌트의 내부 변수를 직접 만지지 않도록,
외부에 노출하는 함수는 의미 있는 동사 (Heal, ApplyDamage 같은 동작 단어) 만 사용 합니다.

- Character 가 TakeDamage 를 받으면 그대로 ApplyDamage 로 위임하며,
- HP 갱신, 다운 판정, 사망 처리는 모두 컴포넌트 안에서 끝냅니다.
- 외부에서 CurrentHP 를 직접 깎지 않습니다. Damage/Heal 함수를 통해서만 조작합니다.

샘플 코드 (UCH4HealthComponent - 체력/다운/사망):

    class UCH4HealthComponent : public UActorComponent
    {
        GENERATED_BODY()

    public:
        UCH4HealthComponent();

        virtual void GetLifetimeReplicatedProps(
            TArray<FLifetimeProperty>& OutLifetimeProps) const override;

        // Character::TakeDamage 가 위임 호출. 서버 권한에서만 HP 갱신/다운 처리.
        float ApplyDamage(float DamageAmount, const FDamageEvent& DamageEvent,
                          AController* EventInstigator, AActor* DamageCauser);

        void Heal(float Amount);                    // 서버 권한에서만 호출
        void Revive(float HealAmount);              // 소생 (서버 권한)

        UFUNCTION(BlueprintCallable, Category = "Player Stat")
        bool IsDowned() const { return bIsDowned; }

        UFUNCTION()
        void OnRep_IsDowned();

    public:
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat", Replicated, meta = (ClampMin = "0.0"))
        float MaxHP = 150.0f;

        UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stat", Replicated)
        float CurrentHP = 150.0f;

        UPROPERTY(ReplicatedUsing = OnRep_IsDowned, VisibleAnywhere, BlueprintReadOnly, Category = "Player Stat")
        bool bIsDowned = false;
    };


--------------------------------------------------------------------------------
4.2 컴포넌트 간 통신은 델리게이트 (이벤트 기반) 로 작성.
--------------------------------------------------------------------------------

컴포넌트가 다른 컴포넌트나 자기 부모(Owner) 를 직접 호출하면,
두 클래스가 서로의 타입을 알아야 해서 한쪽이 바뀌면 다른 쪽도 같이 수정해야 합니다.
"이런 일이 일어났다" 는 사실만 알리고, 듣고 싶은 쪽이 자발적으로 구독하는
델리게이트(이벤트) 패턴을 쓰면 강한 의존성을 해결 할 수 있습니다.

이점:
    - 컴포넌트가 Owner 의 타입을 모르므로, 다른 종류의 액터에 붙여도 그대로
      동작합니다.
    - 한 이벤트를 여러 곳 (게임플레이 로직 + UI + 사운드) 에서 자유롭게 들을
      수 있습니다.
    - BlueprintAssignable 로 만들면 BP 측에서도 이벤트에 노드를 연결할 수
      있습니다.

권장 델리게이트 명명:
    OnXxxRequested  - "이런 일을 처리해 달라" (컴포넌트 -> Owner 요청)
    OnXxxChanged    - "값이 바뀌었다" (HP, Phase 등)
    OnXxxStarted    - "이벤트가 시작됐다"
    OnXxxFinished   - "이벤트가 끝났다"

문제 (직접 호출 - 컴포넌트가 부모 타입을 직접 알아야 함):

    void UCH4InteractionComponent::TryReviveNearbyPlayer()
    {
        // 컴포넌트가 ACH4PlayerCharacter 타입을 알고 직접 호출
        if (ACH4PlayerCharacter* Owner = Cast<ACH4PlayerCharacter>(GetOwner()))
        {
            Owner->StartReviveTarget(TargetCharacter);   // 강한 의존
        }
    }

가이드: 컴포넌트는 이벤트만 발생시키고, Owner 가 그걸 구독해서 처리합니다.

    // 컴포넌트 헤더 - 델리게이트 정의
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCH4OnPickupRequested);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCH4OnReviveRequested, ACharacter*, TargetCharacter);

    UCLASS(...)
    class UCH4InteractionComponent : public UActorComponent
    {
    public:
        UPROPERTY(BlueprintAssignable, Category = "Interaction")
        FCH4OnPickupRequested OnPickupRequested;

        UPROPERTY(BlueprintAssignable, Category = "Interaction")
        FCH4OnReviveRequested OnReviveRequested;

    protected:
        bool TryReviveNearbyPlayer();    // 본문에서 OnReviveRequested.Broadcast(...) 호출
        bool TryPickupNearbyItem();      // 본문에서 OnPickupRequested.Broadcast() 호출
    };

    // 컴포넌트 cpp - 결과만 알림 (누가 듣는지는 모름)
    bool UCH4InteractionComponent::TryReviveNearbyPlayer()
    {
        ACharacter* Target = /* 근처 다운된 동료 탐색 */;
        if (!Target) return false;

        OnReviveRequested.Broadcast(Target);   // Owner 가 누구든 자기 방식으로 처리
        return true;
    }

    // Character 가 BeginPlay 에서 구독
    void ACH4PlayerCharacter::BeginPlay()
    {
        Super::BeginPlay();

        if (InteractionComponent)
        {
            InteractionComponent->OnPickupRequested.AddDynamic(this, &ACH4PlayerCharacter::OnPickupRequestedFromInteraction);
            InteractionComponent->OnReviveRequested.AddDynamic(this, &ACH4PlayerCharacter::OnReviveRequestedFromInteraction);
        }

        if (HealthComponent)
        {
            HealthComponent->OnPlayerDowned.AddDynamic(this, &ACH4PlayerCharacter::OnPlayerDownedFromHealth);
        }
    }


--------------------------------------------------------------------------------
4.3 CharacterMovementComponent 확장 (커스텀 클라이언트 예측)
--------------------------------------------------------------------------------

(Optional) *클라이언트 이동 예측에 대한 구현 및 사용은 어느정도
언리얼 엔진 개발에 익숙해진 이후 적용해 보시는 것을 추천 드립니다.

스프린트나 정조준 이동 같이 "내가 이 상태로 움직이고 싶다" 는 의도를
단순히 복제 변수 + Server RPC 로만 처리하면,
클라이언트가 키를 누르고 서버에 도달했다 돌아올 때까지 (RPC 한 번 왕복하는 시간만큼)
반응이 늦어집니다.

UCharacterMovementComponent 를 확장해서 의도 플래그를 압축 비트 형태로
저장하면, 클라이언트가 즉시 반응하면서도 서버가 같은 입력을
다시 계산해서 결과를 검증할 수 있습니다.

개선 가능 부분:
- 클라이언트가 스프린트 키를 눌러도, 서버 RPC 가 한 번 왕복한 후에야
  속도가 바뀝니다 (네트워크 지연만큼 반응이 늦어집니다).
- 클라이언트의 자체 이동 예측이 서버 결과와 어긋나서,
  서버가 클라이언트의 위치를 강제로 맞추는 보정
  (캐릭터가 살짝 끌려가는 현상) 이 생깁니다.

개선시 이점:
- 클라이언트가 키를 누른 즉시 속도가 바뀌어 반응이 빠릅니다.
- 서버가 클라이언트의 입력 의도를 받아 동일하게 다시 계산하므로, 서버가
  최종 결정권을 가지면서도 클라이언트 캐릭터가 보정으로 끌려가는 현상이 생기지 않습니다.
- 별도 복제 변수나 Server RPC 호출이 필요 없어 네트워크 트래픽이 적게 듭니다.

언제 이 패턴을 쓰면 좋은지:
- 이동 속도/방향에 영향을 주는 의도 (스프린트, 정조준 이동, 웅크리기,
  점프 변형 등) 에 적합합니다.
- 자주 토글되는 boolean 의도에 좋습니다. 단발성 이벤트 (수류탄 던지기 한 번) 는
  일반 Server RPC 가 더 적절합니다.

현재 구현 (단순 Server RPC + OnRep):

    UFUNCTION(Server, Reliable)
    void Server_SetSprinting(bool bNew);
    void Server_SetSprinting_Implementation(bool bNew)
    {
        bIsSprinting = bNew;
        GetCharacterMovement()->MaxWalkSpeed = bNew ? SprintSpeed : DefaultWalkSpeed;
    }

    UFUNCTION()
    void OnRep_IsSprinting();   // 다른 클라이언트들이 보는 동기화

    UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
    bool bIsSprinting;

개선 가이드: UCharacterMovementComponent 는 자체적으로 1 바이트짜리 압축 플래그
슬롯 (FLAG_Custom_0/1 등) 을 제공합니다. 이 슬롯에 의도 플래그를 넣어 서버로
보내면, 클라이언트는 즉시 적용해서 움직이고, 서버는 같은 입력을 받아 동일한 계산을 다시 수행합니다.

    // 헤더
    UCLASS(ClassGroup = (CH4), meta = (BlueprintSpawnableComponent))
    class UCH4MovementComponent : public UCharacterMovementComponent
    {
        GENERATED_BODY()

    public:
        virtual float GetMaxSpeed() const override;
        virtual void UpdateFromCompressedFlags(uint8 Flags) override;
        virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

        void StartSprint();    // bWantsToSprint = true
        void StopSprint();
        void SetAimWalk(bool bAim);

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (ClampMin = "0.0"))
        float SprintSpeed = 600.0f;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (ClampMin = "0.0"))
        float AimWalkSpeed = 220.0f;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Speed", meta = (ClampMin = "0.0"))
        float DefaultWalkSpeed = 350.0f;

        bool bWantsToSprint = false;
        bool bWantsAimWalk = false;
    };

    // FSavedMove 동반 정의 (이번 틱의 입력 의도를 저장하는 객체)
    class FSavedMove_CH4 : public FSavedMove_Character
    {
    public:
        typedef FSavedMove_Character Super;

        uint8 bSavedWantsToSprint : 1;
        uint8 bSavedWantsAimWalk : 1;

        virtual void Clear() override;
        virtual uint8 GetCompressedFlags() const override;
        virtual bool CanCombineWith(const FSavedMovePtr& NewMove,
                                    ACharacter* Character, float MaxDelta) const override;
        virtual void SetMoveFor(ACharacter* Character, float InDeltaTime,
                                FVector const& NewAccel,
                                FNetworkPredictionData_Client_Character& ClientData) override;
        virtual void PrepMoveFor(ACharacter* Character) override;
    };

    class FNetworkPredictionData_Client_CH4 : public FNetworkPredictionData_Client_Character
    {
    public:
        FNetworkPredictionData_Client_CH4(const UCharacterMovementComponent& ClientMovement);
        virtual FSavedMovePtr AllocateNewMove() override;
    };

cpp 핵심 부분:

    float UCH4MovementComponent::GetMaxSpeed() const
    {
        if (bWantsAimWalk)  return AimWalkSpeed;
        if (bWantsToSprint) return SprintSpeed;
        return Super::GetMaxSpeed();
    }

    void UCH4MovementComponent::UpdateFromCompressedFlags(uint8 Flags)
    {
        Super::UpdateFromCompressedFlags(Flags);
        bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
        bWantsAimWalk  = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
    }

    uint8 FSavedMove_CH4::GetCompressedFlags() const
    {
        uint8 Result = Super::GetCompressedFlags();
        if (bSavedWantsToSprint) Result |= FLAG_Custom_0;
        if (bSavedWantsAimWalk)  Result |= FLAG_Custom_1;
        return Result;
    }


--------------------------------------------------------------------------------
                  5. 반복되는 바인딩 코드를 매크로로 정리
--------------------------------------------------------------------------------

PlayerCharacter 처럼 입력 액션 자산을 여러 개 불러오고 BindAction 을 여러 번
호출하는 클래스는 구조만 똑같고 값만 다른 코드 (반복적인 형식 코드) 가 빠르게 쌓입니다.
이런 반복 코드를 매크로로 묶어, 의미 있는 부분 (어떤 액션이 어떤 핸들러로 연결되는지)
만 한눈에 보이게 정리 하는 것이 좋습니다.

--------------------------------------------------------------------------------
5.1 문제: ConstructorHelpers 와 BindAction 의 반복 코드
--------------------------------------------------------------------------------

UInputAction 자산을 불러오려면 자산 1 개당 4 줄 (블록 시작 + Finder 변수 +
Succeeded 체크 + 멤버 대입) 이 필요합니다. BindAction 호출은 인자가 5 개로
깁니다. Toggle 입력 (Sprint/Aim 같이 누르고 있는 동안만 활성) 은 Started 와
Completed 두 이벤트에 같은 핸들러를 따로 바인딩해야 합니다. EquipSlot1~4
같이 슬롯 번호만 다른 입력은 동일 패턴을 4 번 반복합니다.

원본 패턴 (자산 1 개당):

    static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(
        TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Move.IA_Move'"));
    if (MoveActionFinder.Succeeded())
    {
        MoveAction = MoveActionFinder.Object;
    }

    static ConstructorHelpers::FObjectFinder<UInputAction> LookActionFinder(
        TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Look.IA_Look'"));
    if (LookActionFinder.Succeeded())
    {
        LookAction = LookActionFinder.Object;
    }

    // ... 자산이 14 개면 같은 패턴이 14 번 반복

원본 패턴 (BindAction 호출, 토글 입력):

    Enhanced->BindAction(SprintAction, ETriggerEvent::Started,   this, &ACH4Character::OnSprintStarted);
    Enhanced->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACH4Character::OnSprintCompleted);
    Enhanced->BindAction(AimAction,    ETriggerEvent::Started,   this, &ACH4Character::OnAimStarted);
    Enhanced->BindAction(AimAction,    ETriggerEvent::Completed, this, &ACH4Character::OnAimCompleted);
    // ... 토글 입력마다 두 줄씩

문제:
    - 자산 경로 문자열이 길어 한 줄이 화면 너비를 넘어갑니다. 에셋 이름만 보고 어떤 패턴이 다른지 찾기 어렵습니다.
    - Started/Completed 페어를 별도 메서드로 분리하면 메서드 수가 빠르게 늘어납니다.
    - EquipSlot 4 개를 4 번 복사하고, 슬롯 번호만 바뀌는 핸들러 메서드가 4 개로 쪼개집니다.
    - 새 입력 추가 시 자산 적재 + 멤버 선언 + 바인딩 + 핸들러까지 4 곳을 동시에 수정해야 합니다.


--------------------------------------------------------------------------------
5.2 가이드: 도메인 매크로로 반복 코드 줄이기
--------------------------------------------------------------------------------

본 프로젝트의 Refactor/Common/CH4Macros.h 가 다음 매크로들을 정의합니다.

    LOAD_INPUT_ACTION             - 멤버 변수에 IA 자산 1 개를 적재
    LOAD_INPUT_ACTION_INTO_MAP    - TMap[Key] 에 IA 자산 1 개를 적재
    BIND_INPUT_ACTION             - 단일 멤버 IA 에 BindAction 호출
    BIND_INPUT_ACTION_PARAMS      - 추가 인자가 박힌 핸들러로 BindAction 호출
    BIND_MAPPED_INPUT_ACTION      - TMap 조회 + BindAction (없으면 조용히 스킵)
    BIND_MAPPED_INPUT_ACTION_PARAMS - TMap 조회 + 추가 인자가 박힌 BindAction

자산 적재 (생성자 안):

    #define INPUT_DIR "/Game/Player/Input/Action"
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Move,       INPUT_DIR, IA_Move)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Look,       INPUT_DIR, IA_Look)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Jump,       INPUT_DIR, IA_Jump)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Sprint,     INPUT_DIR, IA_Sprint)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Aim,        INPUT_DIR, IA_Aim)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Interact,   INPUT_DIR, IA_Interaction)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot1, INPUT_DIR, IA_Equip)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot2, INPUT_DIR, IA_Equip2)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot3, INPUT_DIR, IA_Equip3)
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot4, INPUT_DIR, IA_Equip4)
    #undef INPUT_DIR

바인딩 (SetupPlayerInputComponent 안):

    using EAction = ECH4PlayerInputAction;

    // 단순 바인딩 (Triggered 또는 Started 한 곳에 직접 핸들러 연결)
    BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions,
                             EAction::Move,       Triggered, HandleMoveInput);
    BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions,
                             EAction::Look,       Triggered, HandleLookInput);
    BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions,
                             EAction::Jump,       Triggered, Jump);
    BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions,
                             EAction::Reload,     Started,   OnReload);

    // EquipSlot - 슬롯 번호를 핸들러에 미리 박아 단일 핸들러 한 개로 처리
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::EquipSlot1, Started, Server_EquipWeaponSlot, 0);
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::EquipSlot2, Started, Server_EquipWeaponSlot, 1);
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::EquipSlot3, Started, Server_EquipWeaponSlot, 2);
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::EquipSlot4, Started, Server_EquipWeaponSlot, 3);

    // Toggle - Started 와 Completed 에 같은 핸들러를 다른 bool 인자로 바인딩
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::Sprint, Started,   HandleInputAction, EAction::Sprint, true);
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::Sprint, Completed, HandleInputAction, EAction::Sprint, false);

    // 한 번 누르고 끝 (HandleInputAction 안에서 종류별로 분기)
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::Interact, Started, HandleInputAction, EAction::Interact, true);
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::Heal,     Started, HandleInputAction, EAction::Heal,     true);
    BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions,
                                    EAction::Throw,    Started, HandleInputAction, EAction::Throw,    true);

매크로 정의 (Refactor/Common/CH4Macros.h):

    // 매크로는 블록 스코프({})로 감싸서, 매크로를 여러 번 호출해도
    // 안에 있는 정적(static) Finder 변수가 호출별로 따로따로 만들어지게 합니다.
    #define LOAD_INPUT_ACTION_INTO_MAP(MapMember, KeyValue, AssetPath, AssetName)                       \
        {                                                                                               \
            static ConstructorHelpers::FObjectFinder<UInputAction> Finder(                              \
                TEXT("/Script/EnhancedInput.InputAction'" AssetPath "/" #AssetName "." #AssetName "'"));\
            if (Finder.Succeeded()) MapMember.Add(KeyValue, Finder.Object);                             \
        }

    // BindAction 의 가변 인자 템플릿 오버로드를 그대로 호출합니다.
    // 클래스의 멤버 함수 안에서만 사용합니다 (this 를 참조하기 때문).
    #define BIND_MAPPED_INPUT_ACTION_PARAMS(InputComp, OwnerClass, Map, Key, Event, Method, ...)        \
        do                                                                                              \
        {                                                                                               \
            UInputAction* IA__ = (Map).FindRef(Key);                                                    \
            if (IA__) (InputComp)->BindAction(IA__, ETriggerEvent::Event, this, &OwnerClass::Method, __VA_ARGS__); \
        } while (0)


--------------------------------------------------------------------------------
5.3 매크로 작성 시 주의사항
--------------------------------------------------------------------------------

블록 스코프 / do-while 로 감싸기:

    매크로 본문에 여러 statement (문장) 가 있으면, 매크로를 호출한 곳에서
    if/else 와 잘못 결합되어 의미가 뒤바뀔 수 있습니다. 블록 ({...}) 또는
    do { ... } while (0) 으로 감싸 매크로 호출 한 번이 단일 statement 처럼
    동작하게 합니다.

    static 변수가 들어간 매크로 (ConstructorHelpers::FObjectFinder) 는 반드시
    블록 스코프로 감싸야, 같은 매크로를 여러 번 호출했을 때 호출별로 별도
    변수가 만들어집니다. 그렇지 않으면 두 번째 이후 호출이 첫 번째와 같은
    static 변수를 가리켜 자산 적재가 망가집니다.

매크로 이름은 SCREAMING_SNAKE_CASE:

    함수와 시각적으로 구별되어 매크로의 부작용 (인자가 여러 번 평가될 수
    있음, static 변수가 들어 있을 수 있음, 호출하는 코드 스코프에 영향을 줄
    수 있음) 을 사용자가 의식하기 쉬워집니다.

임시 prefix 매크로는 #undef 로 정리:

    #define INPUT_DIR "/Game/Player/Input/Action"
    LOAD_INPUT_ACTION_INTO_MAP(InputActions, EAction::Move, INPUT_DIR, IA_Move)
    // ...
    #undef INPUT_DIR

    INPUT_DIR 처럼 한 함수 안에서만 쓰는 짧은 이름 매크로는 사용한 직후
    #undef 로 해제합니다. 정리하지 않으면 그 매크로 정의가 파일 전역에
    남아, 다른 파일에서 같은 이름의 식별자를 쓸 때 충돌합니다.

매크로 폐기 시점을 헤더에 명시:

    매크로는 임시 단계의 도구입니다. 사용하는 클래스가 EditDefaultsOnly
    UPROPERTY 패턴으로 전환되거나 자산 시스템이 정비되면 매크로는 일괄
    제거되어야 합니다. 헤더 상단에 의도와 폐기 조건을 명시합니다.

    // TODO: 본 매크로들은 사용 클래스가 EditDefaultsOnly UPROPERTY 패턴으로
    //       전환되면 일괄 제거 예정.
    #pragma once

매크로 안의 임시 변수 명명 규칙:

    매크로 본문에 등장하는 임시 변수 (예: IA__) 는 호출한 코드의 변수 이름과
    겹치지 않도록 짓습니다. 끝에 언더스코어 두 개를 붙이는 식으로 일반
    식별자가 잘 쓰지 않을 형태를 씁니다. 호출하는 코드의 변수명과 같은
    이름이면 컴파일 에러가 나거나 의도와 다른 변수가 가려집니다.


--------------------------------------------------------------------------------
5.4 매크로 vs 다른 대안은?
--------------------------------------------------------------------------------

매크로 vs 함수:
    ConstructorHelpers::FObjectFinder 는 호출별로 별도 static 변수가 필요하기 때문에
    함수로 감쌀 수 없습니다. 함수로 감싸면 모든 호출이 함수 안의 같은 static 변수 하나만
    공유해서, 처음 호출한 자산만 가리키게 됩니다. 이런 경우는 매크로가 적합합니다.

    BindAction 은 가변 인자 템플릿 + 멤버 함수 포인터를 받는 형태라 일반
    함수로 감싸기 어렵습니다. 함수로 만들려면 호출하는 쪽에서 템플릿 인자를
    명시적으로 적어야 해서 오히려 더 길어집니다. 매크로가 더 짧습니다.

매크로 vs 데이터에셋:
    데이터에셋 경로 자체는 매크로보다 EditDefaultsOnly UPROPERTY 가 더 좋은
    해결책입니다. 자산 경로가 코드에 박혀 있으면 자산 이동, 리네임 시 코드를
    다시 컴파일해야 하지만, UPROPERTY 로 노출하면 BP 단계에서 다시 연결만 하면 됩니다.

    매크로는 입력 시스템이 완전히 정비되기 전 임시 압축 도구이며,
    입력 액션을 DataAsset 으로 묶거나 EditDefaultsOnly TMap 을 BP_Player 의
    default 로 설정하는 방식으로 전환되면 매크로는 제거하는 것이 좋습니다.

매크로 vs 코드 생성:
    같은 반복 코드가 5 회 이상 등장하고 패턴이 정형화되어 있으면 매크로 또는
    데이터 주도 방식이 답입니다. 1~2 회 반복은 매크로 도입의 비용
    (가독성 저하, 디버거 추적 어려움, 컴파일 에러 메시지가 매크로 내부를 가리킴)
    이 더 크니 인라인으로 두는 편이 낫습니다.

언제 매크로를 쓰고 언제 안 쓰는지:

    쓰는 경우:
        - ConstructorHelpers 처럼 static 변수가 호출별로 분리되어야 하는 경우.
        - 호출하는 코드가 5 회 이상 반복되고 패턴이 동일한 경우.
        - 가변 인자 템플릿 + 멤버 함수 포인터 같이 함수로 감싸기 어려운 호출.

    안 쓰는 경우:
        - 데이터 에셋의 경로 / 설정 값 등 EditDefaultsOnly UPROPERTY 로 노출하면 더 나은 것.
        - 디버거에서 한 줄씩 따라가야 하는 비즈니스 로직 (매크로는 디버거가 내부를 들여다보기 어렵습니다).
        - 1~2 회 반복. 인라인으로 두는 편이 가독성이 좋습니다.


--------------------------------------------------------------------------------
              6. 도메인 트리 분리: 장비 시스템과 스킬 시스템
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
6.1 문제: 장비와 스킬이 하나의 Item 트리에 섞여 있음
--------------------------------------------------------------------------------

원본 구조:
    ABaseItem                              (모든 아이템의 부모)
    +-- AEquippable                        (장비 - 무기 종류)
    |   +-- AMeleeWeapons                  (근접 무기)
    |   +-- ARangedWeapons                 (원거리 무기)
    +-- AConsumableItem                    (소비 - 빈 클래스)
    |   +-- AHealItem
    |   +-- AGearItem
    |   +-- AAmmoItem
    +-- AThorwbleItems                     (수류탄 - 어디 소속인지 모호)

문제점:
    - 장비 (무기) 와 소비 (회복/수류탄) 는 완전히 다른 시스템인데 한 트리 안에 섞여 있습니다.
    - ABaseItem 헤더에 무기 전용 멤버
     (WeaponPoseType, CamShakeClass, InnerRadius, OuterRadius) 가 들어있습니다.
      무기와 무관한 모든 아이템도 이 멤버를 강제로 갖게 됩니다.
    - InventoryComponent / SkillComponent / EquippableComponent 가 같은 부모
      타입을 참조해야 해서 캐스트 (타입 변환) 가 자주 발생합니다.
    - 디자이너 입장에서 "수류탄을 던지는 액션" 과 "총을 쥐는 액션" 이 같은
      트리에 있어서 구별이 어렵습니다.


--------------------------------------------------------------------------------
6.2 가이드: 시스템 단위로 분리
--------------------------------------------------------------------------------

개선 구조:
    ACH4ItemBase                           (소비/투척 아이템 - 줍기 + 효과 적용 + 던지기 대상)
    +-- ACH4AmmoItem                       (줍기 -> 무기 탄약 보충)
    +-- ACH4HealItem (계획)                 (줍기 -> 회복 차지 +1)
    +-- ACH4GearItem (계획)                 (줍기 -> 기어 부품 +1)
    +-- ACH4ThrowableItem                  (수류탄 - 던지기 가능한 액터)

    ACH4WeaponBase                         (장비 - 슬롯에 장착해서 들고 다니는 무기)
    +-- ACH4MeleeWeapon
    +-- ACH4RangedWeapon

장비 시스템 추가:
    - ACH4WeaponBase 는 ACH4ItemBase 의 자식이 아닙니다. 별도 최상위 트리로 분리합니다.
    - Equip(OwnerCharacter) / Unequip() / Server_Attack() / Server_Reload() API 를 보유합니다.
    - 무기 데이터는 UCH4WeaponData (UPrimaryDataAsset) 에 외부화합니다.
    - PlayerCharacter 가 4 슬롯 배열 (TArray<ACH4WeaponBase*>) 로 보유 합니다.

스킬 시스템 추가:
    - 회복/투척은 UCH4SkillComponent 가 차지 카운트 (HealItemCount, GrenadeCount) 로 관리합니다.
    - ACH4ItemBase 는 "줍는 순간 스킬 차지를 올리는 액터" 의 부모 클래스입니다.
    - Server_UseHealItem / Server_ThrowGrenade RPC 는 SkillComponent 안에만 존재합니다.

부모에서 무기 전용 멤버 제거 후의 ACH4ItemBase:

    UCLASS()
    class ACH4ItemBase : public AActor
    {
        GENERATED_BODY()

    public:
        ACH4ItemBase();

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
        TObjectPtr<UCH4ItemData> ItemData;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component")
        TObjectPtr<USceneComponent> SceneComponent;

        UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component")
        TObjectPtr<UStaticMeshComponent> MeshComponent;
    };

ACH4WeaponBase (장비 전용 멤버 보유):

    UCLASS()
    class ACH4WeaponBase : public AActor
    {
        GENERATED_BODY()

    public:
        UFUNCTION(BlueprintCallable, Category = "Weapon|Equip")
        void Equip(ACharacter* OwnerCharacter);

        UFUNCTION(BlueprintCallable, Category = "Weapon|Equip")
        void Unequip();

        virtual int32 GetMaxClip() const { return 0; }
        virtual int32 GetMaxAmmo() const { return 0; }
        virtual void AddMaxClip(int32 AmmoItem) {}

        UFUNCTION(Server, Reliable) void Server_Attack();
        UFUNCTION(Server, Reliable) void Server_Reload();

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data", ReplicatedUsing = OnRep_WeaponData)
        TObjectPtr<UCH4WeaponData> WeaponDataAsset;

        UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ammo")
        int32 MaxClip = 0;

        UPROPERTY(Replicated, BlueprintReadOnly, Category = "Ammo")
        int32 CurrentAmmo = 0;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects")
        TSubclassOf<UCameraShakeBase> CamShakeClass;

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effects", meta = (ClampMin = "0.0"))
        float InnerRadius = 500.0f;
    };

ACH4ItemData (효과를 데이터로 외부화):

    UCLASS()
    class UCH4ItemData : public UPrimaryDataAsset
    {
        GENERATED_BODY()

    public:
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
        float Value = 0.0f;     // 회복량, 탄약량 등

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
        EEffectType Type = EEffectType::Health;   // Ammo/Health/Gear/Grenade
    };

ACH4SkillComponent::ApplyItemEffect (줍기 처리):

    void UCH4SkillComponent::ApplyItemEffect(UCH4ItemData* Data)
    {
        if (!Data) return;

        ACharacter* OwnerChar = GetOwnerCharacter();
        if (!OwnerChar) return;

        switch (Data->Type)
        {
        case EEffectType::Ammo:
            // 부착된 무기를 찾아서 탄약 보충
            for (AActor* Actor : OwnerChar->GetAttachedActors())
            {
                if (ACH4WeaponBase* Weapon = Cast<ACH4WeaponBase>(Actor))
                {
                    Weapon->AddMaxClip(Data->Value);
                    return;
                }
            }
            break;

        case EEffectType::Health:  HealItemCount++; break;
        case EEffectType::Gear:    /* GameState 에 기어 카운트 +1 */ break;
        case EEffectType::Grenade: GrenadeCount++; break;
        }
    }


--------------------------------------------------------------------------------
6.3 기능(시스템) 기준으로 분리시 이점
--------------------------------------------------------------------------------

"이 트리에 어떤 클래스들이 자식으로 들어올 수 있는가" 를 기준으로
최상위 부모 클래스를 정합니다.
ACH4ItemBase 의 자식이 모두 "줍는 액터" 라면, 무기는 다른 트리에 두는 것이 맞습니다.
부모 클래스의 책임은 모든 자식 클래스의 공통 책임 (교집합) 이어야 합니다.

- ACH4ItemBase 가 진짜 공통 책임 (메시, 씬 컴포넌트, ItemData) 만 갖게 됩니다.
  모든 아이템이 자기와 무관한 무기 멤버를 떠안지 않게 됩니다.
- 장비 슬롯 시스템과 스킬 차지 시스템이 서로의 클래스를 모르고도 동작합니다.
- 무기 추가 시 ACH4WeaponBase 의 자식만 만들면 됩니다. 아이템 트리에 영향이 없습니다.
- 새 스킬 종류 (예: 부스터, 함정) 추가 시 EEffectType 에 한 줄을 추가하고
  UCH4SkillComponent 의 분기 한 곳만 추가하면 됩니다.

--------------------------------------------------------------------------------
                       7. 엔진 베이스 클래스 선택
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
7.1 AGameMode 와 AGameModeBase 의 선택
--------------------------------------------------------------------------------

AGameMode 는 매치 진행 단계를 자동으로 관리하는 상태 머신
(MatchState::WaitingToStart -> InProgress -> WaitingPostMatch) 을 갖습니다.
이 상태 머신을 사용하지 않을 거라면 더 가벼운 AGameModeBase 를 쓰는 편이 낫습니다.

기준:
- 매치 상태를 사용함 (HandleMatchHasStarted, HasMatchStarted,
  IsMatchInProgress 같은 함수 호출)        -> AGameMode
- 직접 ServerTravel 호출 + 자체 카운팅으로 흐름 제어
                                         -> AGameModeBase

문제: 본 프로젝트는 매치 상태 머신을 안 쓰는데 AGameMode 를 부모 클래스로 사용하고 있습니다.

    class ACH4GameMode : public AGameMode { /* MatchState 미사용 */ };

가이드:
    class ACH4DefenseGameMode : public AGameModeBase
    {
        GENERATED_BODY()

    public:
        ACH4DefenseGameMode();

        virtual void BeginPlay() override;
        virtual void PostLogin(APlayerController* NewPlayer) override;
    };

AGameStateBase / AGameState 도 같은 기준으로 선택합니다. AGameModeBase 와
AGameStateBase 를 짝으로 쓰면 일관됩니다.


--------------------------------------------------------------------------------
7.2 GameMode 생성자에서 Class 변수를 명시합니다
--------------------------------------------------------------------------------

각 GameMode 가 자기 도메인의 PC/GameState/PlayerState/Pawn 클래스를
코드 단계에서 결합합니다. BP 가 누락되거나 잘못 설정됐을 때
엔진 기본값으로 폴백되는 위험을 줄여줍니다.

가이드:

    ACH4DefenseGameMode::ACH4DefenseGameMode()
    {
        GameStateClass        = ACH4DefenseGameState::StaticClass();
        PlayerStateClass      = ACH4DefensePlayerState::StaticClass();
        PlayerControllerClass = ACH4DefensePlayerController::StaticClass();

        bUseSeamlessTravel = false;

        static ConstructorHelpers::FClassFinder<ACH4PlayerCharacter>
            PlayerCharacter(TEXT("/Game/Player/PlayerBluePrint/BP_Player.BP_Player_C"));
        if (PlayerCharacter.Succeeded())
        {
            DefaultPawnClass = PlayerCharacter.Class;
        }
    }


--------------------------------------------------------------------------------
                  8. UPROPERTY 와 UFUNCTION 사용법
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
8.1 모든 UPROPERTY 에 Category 지정
--------------------------------------------------------------------------------

Category 가 없으면 에디터의 디테일 패널에서 분류 그룹이 안 만들어지고,
Blueprint 사용자가 이 멤버가 어디에 속하는지 한눈에 파악하기 어렵습니다.

문제:

    UPROPERTY(EditAnywhere)
    float BaseDamage = 10.f;

    UPROPERTY(VisibleAnyWhere, BlueprintReadWrite)   (오타: AnyWhere)
    int32 Score;

    UPROPERTY(BlueprintReadOnly)
    int32 DayCount;

가이드:

    UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (ClampMin = "0.0"))
    float BaseDamage = 10.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score = 0;

    UPROPERTY(BlueprintReadOnly, Category = "DayPhase")
    int32 DayCount = 0;


--------------------------------------------------------------------------------
8.2 수치 UPROPERTY 에는 ClampMin/Max
--------------------------------------------------------------------------------

    UPROPERTY(EditDefaultsOnly, Category = "Combat", meta = (ClampMin = "0.0"))
    float BaseDamage = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Wave", meta = (ClampMin = "0", ClampMax = "999"))
    int32 MaxZombies = 50;

음수 데미지나 9999 마리 좀비 같이 디자이너 실수로 비정상 값이 들어가는 것을 에디터 단계에서 막아줍니다.


--------------------------------------------------------------------------------
8.3 BlueprintReadWrite 는 정말 필요할 때만
--------------------------------------------------------------------------------

기본은 BlueprintReadOnly 입니다. Blueprint 가 직접 그 멤버를 쓰는 일이 명확할 때만
BlueprintReadWrite 로 노출합니다.

안 좋음: BP 가 임의로 HP 를 직접 설정 가능 -> 게임 규칙 우회 위험.

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
    float CurrentHP = 100.f;

권장: BP 에서는 읽기만 하고, 쓰기는 Damage/Heal 함수를 통해서만 하도록 합니다.

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHP = 100.f;

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float Amount);


--------------------------------------------------------------------------------
8.4 UPROPERTY(Replicated) 4단계 세트
--------------------------------------------------------------------------------

복제 변수는 4 단계가 모두 갖춰져야 동작합니다. 하나라도 빠지면 컴파일은
통과하지만 런타임에 조용히 실패해서 디버깅이 어렵습니다.

문제 사례 (등록 누락):

    class ACH4PlayerState : public APlayerState
    {
    public:
        UPROPERTY(Replicated)
        int32 SpawnPointIndex;     (GetLifetimeReplicatedProps 에 등록 안 됨 -> 복제 안 됨)
    };

    void ACH4PlayerState::GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(ACH4PlayerState, LifeState);
        DOREPLIFETIME(ACH4PlayerState, PlayerReviveCount);
        // SpawnPointIndex 누락
    }

가이드:

    // 1. UPROPERTY 선언 (ReplicatedUsing 또는 Replicated)
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHP, BlueprintReadOnly, Category = "Health")
    float CurrentHP = 100.f;

    // 2. OnRep 함수 선언 (ReplicatedUsing 인 경우만)
    UFUNCTION()
    void OnRep_CurrentHP();

    // 3. GetLifetimeReplicatedProps 에 등록
    void UCH4HealthComponent::GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME(UCH4HealthComponent, CurrentHP);
    }

    // 4. OnRep 본문 (실제로 의미 있는 동작. 비어 있지 않게)
    void UCH4HealthComponent::OnRep_CurrentHP()
    {
        OnHealthChanged.Broadcast(CurrentHP);   // 클라이언트 UI 갱신 등
    }


--------------------------------------------------------------------------------
8.5 본인만 알아도 되는 데이터는 COND_OwnerOnly
--------------------------------------------------------------------------------

다른 플레이어가 알 필요 없는 데이터 (인벤토리 슬롯 같이 본인만 쓰는 정보)
는 대역폭 절약을 위해 OwnerOnly 조건으로 한정합니다.

    void UCH4SkillComponent::GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps) const
    {
        Super::GetLifetimeReplicatedProps(OutLifetimeProps);
        DOREPLIFETIME_CONDITION(UCH4SkillComponent, HealItemCount, COND_OwnerOnly);
        DOREPLIFETIME_CONDITION(UCH4SkillComponent, GrenadeCount,  COND_OwnerOnly);
    }


--------------------------------------------------------------------------------
                       9. 포인터/참조 타입 표기
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
9.1 멤버 포인터는 TObjectPtr
--------------------------------------------------------------------------------

언리얼 5 부터 TObjectPtr<T> 는 가비지 컬렉션과 잘 동작하며,
디버거에서 객체 상태를 추적하기 쉽습니다.
Epic 에서 공식적으로 적극 적인 사용을 추천 하고 있습니다.

문제:

    class ACH4GameState : public AGameState
    {
        UPROPERTY()
        class ADirectionalLight* DirectionalLight;  (raw 포인터)

        UPROPERTY()
        USoundBase* ZombieSound;                    (raw 포인터)
    };

가이드:

    class ACH4DefenseGameState : public AGameStateBase
    {
        UPROPERTY()
        TObjectPtr<ADirectionalLight> DirectionalLight;

        UPROPERTY(EditAnywhere, Category = "Sound")
        TObjectPtr<USoundBase> ZombieSound;
    };


--------------------------------------------------------------------------------
9.2 약참조는 TWeakObjectPtr (메모리릭 방지)
--------------------------------------------------------------------------------

직접 소유하지 않고 단순히 참조만 하는 경우, 강참조 (TObjectPtr) 로 두면 그
대상이 가비지 컬렉션 대상이 되어도 우리 쪽이 붙잡고 있어서 메모리에서 해제가 되지 않습니다.

    UPROPERTY()
    TWeakObjectPtr<ACH4DefensePlayerController> CachedController;


--------------------------------------------------------------------------------
9.3 헤더에서는 전방 선언을 우선 사용
--------------------------------------------------------------------------------

헤더 A 가 헤더 B 를 #include 하면, 헤더 A 를 #include 하는 모든 파일이
자동으로 헤더 B 까지 같이 끌어오게 됩니다. 이런 식으로 의존이 번지면 컴파일
시간이 늘고, 헤더 B 가 변경됐을 때 영향받는 파일이 폭증합니다. 포인터나
참조로만 사용하는 타입은 #include 대신 전방 선언 (class XXX;) 으로 충분합니다.

문제 (헤더가 다른 헤더 두 개를 끌고 들어감):

    // CH4DefenseGameState.h
    #include ".../CH4DefenseGameMode.h"
    #include ".../CH4DefensePlayerController.h"

가이드:

    // CH4DefenseGameState.h
    #pragma once

    #include "CoreMinimal.h"
    #include "GameFramework/GameStateBase.h"
    #include "CH4_TeamProject/DataBase/DataBase.h"   // enum 은 값으로 쓰니 include 필수
    #include "CH4DefenseGameState.generated.h"

    class ADirectionalLight;        // 포인터로만 쓰면 전방 선언으로 충분
    class ASkyLight;
    class AExponentialHeightFog;
    class USoundBase;

    UCLASS()
    class ACH4DefenseGameState : public AGameStateBase
    {
        UPROPERTY()
        TObjectPtr<ADirectionalLight> DirectionalLight;
    };

    // CH4DefenseGameState.cpp 에서만 실제 #include
    #include "CH4DefenseGameState.h"
    #include "Engine/DirectionalLight.h"
    #include "Components/LightComponent.h"


--------------------------------------------------------------------------------
                            10. 멀티플레이 패턴
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
10.1 상태 변경은 서버에서만, 입력 처리는 본인 클라이언트에서만
--------------------------------------------------------------------------------

게임 상태를 바꾸는 함수는 서버에서만, 사용자 입력을 처리하는 함수는 그
입력을 실제로 받은 본인 클라이언트에서만 실행되어야 합니다. 함수 진입부에
가드 (조건 검사) 를 둬서 다른 곳에서 호출되면 곧바로 빠져나가게 합니다.

    // 서버 권한에서 실행되어야 하는 함수
    void ACH4DefenseGameMode::OnPlayerDowned(ACH4DefensePlayerState* PlayerState)
    {
        if (!HasAuthority()) return;     (서버 권한 확인)
        if (!PlayerState) return;        (인자 nullptr 확인)
        // ...
    }

    // 본인이 조작하는 클라이언트에서만 실행되어야 하는 함수
    void ACH4PlayerCharacter::OnAimInput()
    {
        if (!IsLocallyControlled()) return;   (본인 컨트롤러 확인)
        Server_StartAim();
    }


--------------------------------------------------------------------------------
10.2 GetFirstPlayerController 금지 (서버 측 코드에서)
--------------------------------------------------------------------------------

OnRep_ 함수는 기본적으로 서버에서 호출되지 않지만, 코드 흐름을 위해 서버에서 직접 호출 하는 경우,
"첫 번째 PlayerController" 라는 개념이 의미가 없습니다.
데디케이트(전용) 서버에서는 서버에 직접 붙은 플레이어가 없어서 GetFirstPlayerController 가
nullptr 를 반환하게 되어 크래시가 발생 할 가능성이 높아 집니다.

참고: 클라이언트 측 OnRep 같이 한 클라이언트당 PlayerController 가 1 개만
있는 컨텍스트에서는 GetFirstPlayerController 가 안전한 경우도 있습니다.
서버에서 호출되는 함수에서만 이 함수 사용을 절대 피해 주세요.

서버에서 OnRep_ 함수 직접 호출시 문제:

    void ACH4GameState::OnRep_GamePhase()
    {
        ACH4PlayerController* PC = Cast<ACH4PlayerController>(
            GetWorld()->GetFirstPlayerController());
        if (!PC) return;
        PC->ShowGameClear();
    }

가이드 (서버에서 모든 플레이어에게 알릴 때):

    void ACH4DefenseGameMode::EndGame(EGamePhase GP)
    {
        if (!HasAuthority()) return;

        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
             It; ++It)
        {
            if (ACH4DefensePlayerController* PC =
                Cast<ACH4DefensePlayerController>(It->Get()))
            {
                PC->Client_DisablePlayerInput();
            }
        }
    }


--------------------------------------------------------------------------------
10.3 RPC 명명 일관성
--------------------------------------------------------------------------------

같은 RPC 인데 prefix (접두어) 가 섞이면 코드 전체에서 RPC 만 골라서 보기 어렵습니다.
(검색 에서 일부만 잡힙니다)
    
Server_*  = Server RPC. 가능한 한 WithValidation 동반.
Multi_*   = NetMulticast RPC. 시각/사운드 효과 위주.
Client_*  = Client RPC.
계속 유지되는 상태는 NetMulticast 가 아닌 복제 변수 + OnRep 으로 동기화합니다.

문제:

    UFUNCTION(Server, Reliable)        void StartGame();
    (Server_ prefix 누락)

    UFUNCTION(NetMulticast, Reliable)  void HealLog(float Amount);
    (Multi_ prefix 누락)

    UFUNCTION(NetMulticast, Reliable)  void UpdateCombatPose(...);
    (Multi_ prefix 누락)

    UFUNCTION(Client, Reliable)        void Client_PlayDownAnim();
    (OK)

가이드:

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_StartGame();
    bool Server_StartGame_Validate();   // 의미 있는 검증 함수

    UFUNCTION(NetMulticast, Reliable)
    void Multi_HealLog(float Amount);

    UFUNCTION(NetMulticast, Reliable)
    void Multi_UpdateCombatPose(ECombatPose NewPose);

    UFUNCTION(Client, Reliable)
    void Client_PlayDownAnim();


--------------------------------------------------------------------------------
10.4 NetMulticast 와 복제 변수 중 무엇을 쓸지
--------------------------------------------------------------------------------

문제 (NetMulticast 로 계속 유지되는 상태를 전달):

    UFUNCTION(NetMulticast, Reliable)
    void Multi_SetIsDowned(bool bNewDowned);

    void Multi_SetIsDowned_Implementation(bool bNewDowned)
    {
        bIsDowned = bNewDowned;   // 호출 시점에 접속해 있던 클라이언트만 받음
                                  // 그 후에 접속한 클라이언트는 못 받음
    }

가이드:

    UPROPERTY(ReplicatedUsing = OnRep_IsDowned, BlueprintReadOnly)
    bool bIsDowned = false;

    UFUNCTION()
    void OnRep_IsDowned();   // 나중에 접속한 클라이언트도 자동으로 현재 값을 받음

NetMulticast 는 호출하는 그 순간에 접속해 있던 클라이언트에만 도달합니다.
따라서 일회성 이벤트 (폭발 효과, 한 번 재생되는 사운드) 에만 적합합니다.
계속 유지되어야 하는 상태 (다운 여부, HP, 게임 단계) 는 복제 변수로
만들어야, 나중에 접속한 플레이어도 현재 값을 자동으로 받을 수 있습니다.


--------------------------------------------------------------------------------
                  11. 데이터 자산화와 매직 넘버 제거
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
11.1 매직 넘버를 함수 인자로 넘기지 않고 별도의 데이터로 구성
--------------------------------------------------------------------------------

별도의 데이터로 구성 했을때의 이점:
- 디자이너가 코드 빌드 없이 wave 균형을 조정할 수 있습니다.
- 단계별 차이를 자산 비교로 한눈에 볼 수 있습니다.
- 새 wave 종류를 추가할 때 함수 시그니처를 바꾸지 않고 자산 하나만
  추가하면 됩니다.

문제 (6 개 숫자가 무엇을 의미하는지 함수 시그니처만 봐서는 알 수 없음):

    void SpawnZombie(int32 NormalMin, int32 NormalMax,
                     int32 RunnerMin, int32 RunnerMax,
                     int32 TankMin,   int32 TankMax);

    ZombieSpawnVolume->SpawnZombie(5, 10, 4, 6, 1, 1);
    ZombieSpawnVolume->SpawnZombie(1, 3, 1, 2, 0, 1);

가이드: DataAsset 으로 묶습니다.

    USTRUCT(BlueprintType)
    struct FCH4ZombieSpawnEntry
    {
        GENERATED_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
        TSubclassOf<ACH4MonsterBase> ZombieClass;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0"))
        int32 MinCount = 0;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0"))
        int32 MaxCount = 0;
    };

    UCLASS(BlueprintType)
    class UCH4ZombieSpawnData : public UPrimaryDataAsset
    {
        GENERATED_BODY()

    public:
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
        TArray<FCH4ZombieSpawnEntry> Entries;

        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn", meta = (ClampMin = "0.0"))
        float SpawnRadius = 1500.f;
    };

    class ACH4ZombieSpawnPoint : public AActor
    {
        UFUNCTION(BlueprintCallable, Category = "Spawn")
        void SpawnZombies(UCH4ZombieSpawnData* OverrideData = nullptr);
    };

GameMode 가 단계별 DataAsset 을 노출:

    class ACH4DefenseGameMode : public AGameModeBase
    {
        UPROPERTY(EditDefaultsOnly, Category = "Wave")
        TObjectPtr<UCH4ZombieSpawnData> StageStartWaveData;

        UPROPERTY(EditDefaultsOnly, Category = "Wave")
        TObjectPtr<UCH4ZombieSpawnData> DayWaveData;

        UPROPERTY(EditDefaultsOnly, Category = "Wave")
        TObjectPtr<UCH4ZombieSpawnData> NightWaveData;
    };


--------------------------------------------------------------------------------
11.2 에셋 경로 하드코딩 금지
--------------------------------------------------------------------------------

안 좋음: 에셋 경로 문자열이 코드에 직접 박혀 있습니다.

    static ConstructorHelpers::FObjectFinder<USkeletalMesh>
        MeshAsset(TEXT("/Game/Player/Meshes/SK_Player.SK_Player"));
    GetMesh()->SetSkeletalMesh(MeshAsset.Object);

좋음: UPROPERTY 로 노출해서 BP 에서 할당.

    UPROPERTY(EditDefaultsOnly, Category = "Mesh")
    TObjectPtr<USkeletalMesh> DefaultMesh;

GameMode 의 DefaultPawnClass 처럼 클래스 자체를 가져와야 하는 경우는
ConstructorHelpers::FClassFinder 를 임시로 허용하되, 위치를 BP_Player 같이
잘 알려진 단일 위치로 한정합니다.


--------------------------------------------------------------------------------
11.3 IP/포트 같은 환경 의존 값
--------------------------------------------------------------------------------

안 좋음:

    PC->ClientTravel(TEXT("43.200.5.254:7777"), TRAVEL_Absolute);

좋음: UPROPERTY 로 노출하거나, ini 또는 명령행 인자에서 읽어오도록 합니다.

    UPROPERTY(EditDefaultsOnly, Category = "Network")
    FString ServerAddress = TEXT("43.200.5.254:7777");

이상적으로는 DefaultGame.ini 에서 읽거나 사용자 입력 (접속 메뉴) 으로
받습니다.



--------------------------------------------------------------------------------
                       12. Include 와 의존성 관리
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
12.1 헤더에 함수 본문 최소화
--------------------------------------------------------------------------------

함수 본문이 헤더에 있으면, 그 헤더를 변경할 때마다 그 헤더를 #include 하는
모든 파일이 다시 컴파일됩니다.

안 좋음 (본문이 헤더에):

    class ACH4DefenseGameMode : public AGameModeBase
    {
    public:
        void EndGame(EGamePhase GP)
        {
            bGameStarted = false;
            // ... 30 줄 본문 ...
        }
    };

좋음 (간단한 getter/setter 만 헤더에):

    class ACH4DefenseGameMode : public AGameModeBase
    {
    public:
        void EndGame(EGamePhase GP);                                (cpp 에 본문)

        EGamePhase GetGamePhase() const { return GamePhase; }       (한 줄 getter 는 헤더에 OK)
        void SetGamePhase(EGamePhase NewPhase);                     (setter 는 cpp 에 - OnRep 트리거 같은 부수효과 고려)
    };


--------------------------------------------------------------------------------
12.2 include 순서
--------------------------------------------------------------------------------

    // .cpp 파일
    #include "CH4DefenseGameMode.h"   // 1. 짝 헤더 (자기 자신의 헤더)

    #include ".../CH4DefenseGameState.h"
    #include ".../CH4PlayerCharacter.h"
                                       // 2. 프로젝트 헤더

    #include "Engine/World.h"          // 3. 엔진 헤더
    #include "Kismet/GameplayStatics.h"


--------------------------------------------------------------------------------
12.3 중복 include 제거
--------------------------------------------------------------------------------

안 좋음:

    #include "CH4_TeamProject/DataBase/DataBase.h"
    #include "CH4_TeamProject/Player/PlayerAnimInstance.h"
    #include "CH4_TeamProject/DataBase/DataBase.h"   (중복 - 실제 사례)

IDE 에서 사용 안 한 include 경고를 활성화하고, 작업 종료 전 정리합니다.


--------------------------------------------------------------------------------
12.4 모듈 prefix 긴 경로 정리
--------------------------------------------------------------------------------

안 좋음 (모든 헤더에 모듈 이름이 prefix 로 붙음):

    #include "CH4_TeamProject/Player/CH4Character.h"
    #include "CH4_TeamProject/Game/CH4GameState.h"
    #include "CH4_TeamProject/Item/Equippable/Equippable.h"

가이드: Build.cs 에 다음 한 줄을 추가합니다.

    PublicIncludePaths.AddRange(new string[] { ModuleDirectory });

그 후엔 짧은 경로로 사용할 수 있습니다.

    #include "Player/CH4Character.h"
    #include "Game/CH4GameState.h"
    #include "Item/Equippable/Equippable.h"


--------------------------------------------------------------------------------
                            13. 코드 청결성
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
13.1 빈 클래스/메서드 만들지 않기
--------------------------------------------------------------------------------

사용처 없는 클래스를 미리 만들어 두면,
새로작업을 하거나 다른 사람이 작업을 시작할때에 혼란을 줄 수 있습니다.

문제 사례:

    class UCH4InventoryComponent : public UActorComponent
    {
        UCH4InventoryComponent() {}
        virtual void BeginPlay() override { Super::BeginPlay(); }
        virtual void TickComponent(...) override { Super::TickComponent(...); }
        // 본문 없음. 다른 곳에서 사용 0 건.
    };

가이드: 실제 책임이 정해진 시점에 만들거나, 사용처가 없으면 삭제합니다.

빈 위젯, 빈 컴포넌트, 본문 없는 인터페이스를 placeholder 로 두면 다른
사람이 "이미 누가 쓰고 있겠지" 하고 잘못 가정하기 쉽습니다.


--------------------------------------------------------------------------------
13.2 임시 LogTemp 정리
--------------------------------------------------------------------------------

작업 중에는 OK:

    UE_LOG(LogTemp, Warning, TEXT("야후! ShowGameRule 함수가 일단 실행은 됐어!"));

작업 종료 시: 영역별 로그 카테고리로 옮기거나 삭제합니다.

    DECLARE_LOG_CATEGORY_EXTERN(LogCH4UI, Log, All);
    UE_LOG(LogCH4UI, Log, TEXT("ShowGameRule entered"));

영역별 카테고리 예시:

    DECLARE_LOG_CATEGORY_EXTERN(LogCH4Combat, Log, All);
    DECLARE_LOG_CATEGORY_EXTERN(LogCH4Net,    Log, All);
    DECLARE_LOG_CATEGORY_EXTERN(LogCH4UI,     Log, All);
    DECLARE_LOG_CATEGORY_EXTERN(LogCH4AI,     Log, All);


--------------------------------------------------------------------------------
13.3 주석 처리된 코드 제거
--------------------------------------------------------------------------------

안 좋음 (주석 처리한 코드를 그대로 남겨둠):

    // UFUNCTION(BlueprintCallable, Category = "Menu")
    // void JoinServer();
    UFUNCTION(BlueprintCallable, Category = "GameEvents")
    void ShowGameClear();

    // void ACH4PlayerController::JoinServer()
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("JoinServer Called"));
    //     if (IsLocalController())
    //     {
    //         ClientTravel(TEXT("127.0.0.1:7777"), TRAVEL_Absolute);
    //     }
    // }

가이드: 주석 처리된 코드는 삭제합니다. 이력은 버전 관리 시스템 (Git 등) 이
보관해 줍니다. 본 프로젝트의 ACH4PlayerController.cpp 에 주석 코드 블록이
4 곳 이상 남아있습니다.


--------------------------------------------------------------------------------
13.4 코드를 다시 풀어 쓴 주석 제거
--------------------------------------------------------------------------------

코드 자체로 명확한 내용을 한국어로 다시 풀어 쓴 주석은 가독성을
떨어뜨립니다. 코드와 주석이 항상 맞춰져 있다는 보장이 없어, 코드를 바꿨는데
주석을 안 고치면 거짓 정보가 됩니다.

안 좋음 (코드 동작을 한국어로 그대로 옮겨 쓴 주석):

    // 1. 위젯 생성 (설계도인 Class로 실체인 Instance를 만듭니다)
    CurrentMenuWidget = CreateWidget<UUserWidget>(this, StartMenuClass);
    // 2. 화면에 붙이기
    CurrentMenuWidget->AddToViewport();
    // 3. 마우스 설정: 시작 메뉴에서는 버튼을 눌러야 하니 마우스를 보여줍니다.
    bShowMouseCursor = true;

가이드: 코드 자체로 명확하면 주석을 달지 않습니다.

    CurrentMenuWidget = CreateWidget<UUserWidget>(this, StartMenuClass);
    CurrentMenuWidget->AddToViewport();
    bShowMouseCursor = true;

주석은 "왜 이렇게 했는가" 가 코드만 봐서는 안 보일 때만 답니다.

    // 1초당 1씩 누적되는 게임 시간(초). DayPhase 전환 임계값 비교의 기준.
    UPROPERTY(ReplicatedUsing = OnRep_ServerTime, BlueprintReadOnly)
    float ServerTime = 0.f;


--------------------------------------------------------------------------------
13.5 본문이 같은 중복 함수 통합
--------------------------------------------------------------------------------

안 좋음 (두 함수 본문이 사실상 같음):

    void ACH4PlayerState::SetLifeState(EPlayerLifeState NewState)
    {
        LifeState = NewState;
    }

    void ACH4PlayerState::SetPlayerLifeState(EPlayerLifeState PL)
    {
        LifeState = PL;
    }

가이드: 단일 구현 + 위임 형태로 통합합니다. 기존 BP 가 양쪽 함수를 호출
중이면 한쪽이 다른 쪽을 부르도록 합니다.

    void ACH4DefensePlayerState::SetLifeState(EPlayerLifeState NewState)
    {
        LifeState = NewState;
    }

    UFUNCTION(BlueprintCallable, Category = "Life")
    void SetPlayerLifeState(EPlayerLifeState NewState)   // BP 호환을 위해 유지
    {
        SetLifeState(NewState);
    }


--------------------------------------------------------------------------------
13.6 안 쓰는 멤버 제거
--------------------------------------------------------------------------------

선언만 있고 어디에서도 read/write 가 없는 멤버는 코드를 읽는 사람을
헷갈리게 합니다 ("이 변수가 뭔가 중요한 게 아닐까" 하고 추측하게 됩니다).

안 좋음:

    class ACH4GameState : public AGameState
    {
    private:
        EDayPhase RecentPhase;          (어디에서도 read/write 안 함)

    public:
        EDayPhase FinalDefenceDayPhase = EDayPhase::Night;
        (어디에서도 read 안 함)

        UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Leve2")
        int32 MaxLevels;                (외부에서 사용 0 건)
    };

가이드: 검색으로 사용처를 확인한 후 0 건이면 제거합니다.


--------------------------------------------------------------------------------
13.7 본문이 없는 OnRep 의 의도 명시
--------------------------------------------------------------------------------

본문이 비어 있는 OnRep 함수는 두 가지 의미를 가질 수 있습니다.
(1) 클라이언트가 변수 변경을 알아야 하지만 변수 자동 갱신만으로 충분해서
    즉시 실행할 코드가 없는 경우.
(2) 구현을 깜빡 잊고 비워둔 경우.
코드만 봐서는 어느 쪽인지 구분이 안 됩니다.

안 좋음:

    void ACH4GameState::OnRep_GearPartsCount()
    {
    }

    void ACH4GameState::OnRep_ServerTime()
    {
    }

가이드 1: 의도적으로 비웠다면 한 줄 주석으로 명시합니다.

    // 클라 측에서 즉시 할 일 없음. UI 가 BP 측에서 변수에 직접 바인딩.
    void ACH4DefenseGameState::OnRep_GearPartsCount()
    {
    }

가이드 2: OnRep 자체가 필요 없으면 ReplicatedUsing 대신 Replicated 만 씁니다.

    UPROPERTY(Replicated, BlueprintReadOnly)   (OnRep 없음)
    int32 GearPartsCount = 0;



--------------------------------------------------------------------------------
                  14. 도메인 격리와 캐스트 타입 좁히기
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
14.1 도메인 코드는 자기 도메인 타입으로 캐스트
--------------------------------------------------------------------------------

문제 사례 (현재 도메인이 무엇인지와 무관하게 부모 타입으로 캐스트):

    void ACH4DefenseGameMode::OnPlayerDowned(ACH4PlayerState* PlayerState)
    {
        // 부모 PlayerState 로 받음. 이 함수가 어느 도메인 코드인지 시그니처에서 안 보임.
        if (PlayerState->PlayerReviveCount <= 0) { /* ... */ }

        // 부모 PC 로 캐스트
        if (ACH4PlayerController* PC = Cast<ACH4PlayerController>(PlayerState->GetOwner()))
        {
            PC->Client_PlayDownAnim();
        }
    }

가이드:

    void ACH4DefenseGameMode::OnPlayerDowned(ACH4DefensePlayerState* PlayerState)
    {
        if (!PlayerState) return;
        if (PlayerState->PlayerReviveCount <= 0) { /* ... */ }

        if (ACH4DefensePlayerController* PC =
            Cast<ACH4DefensePlayerController>(PlayerState->GetOwner()))
        {
            PC->Client_PlayDownAnim();
        }
    }

이점:

    - 함수 시그니처만 보고 "DefenseGameMode 가 DefensePlayerState 와
      DefensePlayerController 만 다룬다" 는 것을 한눈에 확인이 가능합니다.
    - 다른 모드 (로비 등) PC 가 실수로 같은 맵에 들어와도 캐스트가 실패해
      nullptr 가 되어 잘못된 호출이 일어나지 않습니다.
    - 후속 리팩토링에서 부모 클래스의 함수를 자식 클래스로 옮겨도, 자식
      타입으로 캐스트했기 때문에 호출 코드를 안 고쳐도 됩니다.


--------------------------------------------------------------------------------
14.2 nullptr 가드는 Cast 직후마다
--------------------------------------------------------------------------------

문제 사례 (가드 없음 - 캐스트가 실패하면 크래시):

    for (FConstPlayerControllerIterator It = ...; It; ++It)
    {
        ACH4PlayerController* PC = Cast<ACH4PlayerController>(It->Get());
        PC->Client_EnablePlayerInput();   // PC 가 다른 타입이면 nullptr -> 크래시
    }

가이드:

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
         It; ++It)
    {
        if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(It->Get()))
        {
            PC->Client_EnablePlayerInput();
        }
    }

if (Type* Var = Cast<Type>(Obj)) 패턴은 캐스트 + nullptr 가드 + 변수 스코프
한정 세 가지를 한 줄로 처리해서 가독성이 좋습니다.


--------------------------------------------------------------------------------
                        15. 리팩토링 작업 방식 가이드
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
15.1 기존 파일 0 줄 수정 원칙
--------------------------------------------------------------------------------

신규 클래스는 Refactor/ 폴더(혹은 그에 대응하는 폴더) 안에 작성하고,
기존 파일은 완료 시점까지 손대지 않습니다.

이유:
    - 기존 클래스를 직접 고치면, 그 클래스를 부모로 하는 BP 자산들이 즉시 영향을 받습니다.
      한 번에 여러 BP 가 깨지면서 한꺼번에 고쳐야 할 양이 너무 많아집니다.
    - 신규 클래스를 옆에 두면 작성 -> 검증 -> 컷오버 단계로 작업을 나눌 수 있습니다.
    - 컷오버 시점에 BP 의 부모 클래스 변경 (reparent) 과 C++ 코드 교체를 한 번에 처리하면,
      BP 가 영향받는 시점을 한 차례로 모을 수 있습니다.

작업 흐름:
    Phase 1 (작성)   - Refactor/ 에 새 클래스 생성. 기존 파일은 손대지 않음.
                      새 클래스는 컴파일 통과하지만 게임에서는 아직 사용되지 않음. BP 영향 없음.
    Phase 2 (검증)   - 새 클래스를 일부 사용처에 시범적으로 연결해서 동작 확인.
                      기존 클래스는 그대로 두고 새 클래스와 병행 가동.
    Phase 3 (컷오버) - 기존 클래스 사용처를 새 클래스로 일괄 교체.
                      기존 클래스 삭제. Refactor/ 안의 파일을 최종 위치로 이동.
                      BP 자산이 새 클래스/위치를 참조하도록 수정.

작업 범위 분류:

    반드시 Refactor/ 사용:
        - 거대 클래스 분리, 부모 클래스 도입 등 클래스를 새로 만들거나 구조를 바꾸는 작업.

    선택 적용:
        - 헤더 안의 멤버 순서만 바꾸는 것처럼, 같은 파일 안에서만 정리하는 작업은
          직접 수정해도 자연스럽습니다.

    직접 수정 (in-place):
        - 파일/폴더명 변경, 식별자 오타 정정, 중복 include 제거 같이 직접 수정하는 것 외에 다른 방법이 없는 작업.


--------------------------------------------------------------------------------
15.2 새 클래스 헤더 상단 리팩토링 주석
--------------------------------------------------------------------------------

Refactor/ 안의 새 클래스 헤더 (.h) 는 #pragma once 위에 출처/변경/사유 주석을 둡니다.
컷오버 후에도 한동안 유지하여, 코드만 보고 어떤 클래스가 어디에서 옮겨왔는지 추적할 수 있게 해줍니다.

    /*
     * [Refactor] ACH4DefenseGameMode
     *
     * 출처:
     *   - Game/CH4GameMode.h/cpp 의 ACH4GameMode (방어전 모드 게임 루프 전체)
     *   - Game/GameManagers/FinalDefenceManager.h/cpp 의 AFinalDefenceManager
     *     (최종 방어 Wave Timer)
     * 변경:
     *   - 두 클래스를 ACH4DefenseGameMode 로 통합
     *   - 부모 AGameMode -> AGameModeBase
     *   - 의존 타입을 모두 Refactor/ 의 새 클래스로 전환
     *   - ZombieSpawn 매직 숫자 제거. UCH4ZombieSpawnData 기반 데이터 주도형
     * 사유: 도메인 명확화 + GameMode 그룹화 + Manager 클래스 패턴 제거
     */
    #pragma once

기재 항목:

    출처   - 어떤 기존 파일/클래스에서 분리되어 왔는지. 여러 곳에서 합쳤다면 모두 나열합니다.
    변경   - 옮겨온 멤버/함수, 합쳐진 책임, 이름이 바뀐 식별자 등 핵심 변동을 불릿으로 요약합니다.
    사유   - 한 줄 의도. 왜 이렇게 변경했는지 적습니다.

#pragma once 위에 두는 이유: 클래스 본체와 시각적으로 분리되어 IDE 폴딩이나
자동 생성 코드와 섞이지 않습니다.


--------------------------------------------------------------------------------
15.3 새 클래스의 의존은 새 클래스끼리만
--------------------------------------------------------------------------------

Refactor/ 안의 새 클래스가 원본 클래스에 의존하면, 컷오버 시점에 의존 그래프가 한 번에 풀리지 않습니다.
새 GameMode 가 새 PC, 새 GameState, 새 PlayerState, 새 PlayerCharacter, 새 SpawnPoint 만 참조하도록
모든 caster/include 를 새 클래스로 좁힙니다.

문제 사례:

    // 새 GameMode 의 cpp 가 원본 클래스를 include
    #include "CH4_TeamProject/Player/CH4Character.h"            (원본)
    #include "CH4_TeamProject/Item/Consumable/ItemSpawnPoint.h" (원본)
    #include "CH4_TeamProject/Zombie/ZombieSpawnPoint.h"        (원본)

가이드: 모든 의존을 Refactor/ 의 새 클래스로 전환합니다.

    #include "CH4_TeamProject/Refactor/Player/Character/CH4PlayerCharacter.h"
    #include "CH4_TeamProject/Refactor/SpawnPoint/CH4ItemSpawnPoint.h"
    #include "CH4_TeamProject/Refactor/SpawnPoint/CH4ZombieSpawnPoint.h"

리팩토링 클래스가 아직 없는 의존이라면, 헤더 주석에 "원본 의존 유지" 사유를
명시하고 후속 작업으로 분리합니다.
