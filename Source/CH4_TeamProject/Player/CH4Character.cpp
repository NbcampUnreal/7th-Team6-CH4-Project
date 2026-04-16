#include "CH4Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "CH4_TeamProject/Game/CH4PlayerState.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "../Item/Equippable/EquippableComponent.h"
#include "CH4_TeamProject/Item/Consumable/HealItem.h"
#include "../Item/Consumable/HealItem.h"
#include "CH4_TeamProject/Player/PlayerAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "CH4_TeamProject/Game/CH4GameState.h"
#include "CH4_TeamProject/Item/Consumable/GearItem.h"


//생성자
ACH4Character::ACH4Character()
{

	PrimaryActorTick.bCanEverTick = true;

	InitializationPlayerMesh();//메쉬 함수 호출
	InitializationCamera();//카메라 함수 호출
	InitializationInput();//인풋 함수

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(TEXT("/Game/Player/PlayerBluePrint/ABP_Player.ABP_Player_C"));
	if (AnimInstance.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	}

	EquippableComponent = CreateDefaultSubobject<UEquippableComponent>(TEXT("EquippableComponent"));
	EquippableComponent->SetIsReplicated(true);
	bReplicates = true;
}

//비긴 플레이
void ACH4Character::BeginPlay()
{
	Super::BeginPlay();
	//맵핑 불러오기
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultContext, 0);
		}
	}
	if (IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("내 이름은 %s이고, 로컬 컨트롤러가 나를 조종 중이다!"), *GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: 난 그냥 원격 캐릭터일 뿐이야."), *GetName());
	}
	GamsState = Cast<ACH4GameState>(GetWorld()->GetGameState());
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	UpdateRotationMode();
}

//틱
void ACH4Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAimCamera(DeltaTime);//조준 카메라 상태 업데이트
}

void ACH4Character::OnEquipInput1()
{
	UE_LOG(LogTemp, Error, TEXT("호출은 성공"));

	if (EquippableComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("실패 원인: EquippableComponent가 비어있음!"));
	}

	if (PrimaryWeaponData1 == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("실패 원인: PrimaryWeaponData가 비어있음! 에디터에서 할당했는지 확인하세요."));
	}

	if (EquippableComponent && PrimaryWeaponData1)
	{
		EquippableComponent->EquipWeapon(PrimaryWeaponData1);
		UE_LOG(LogTemp, Warning, TEXT("장착 성공! 서버 함수 호출함."));
	}
}

void ACH4Character::OnEquipInput2()
{
	if (EquippableComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("실패 원인: EquippableComponent가 비어있음!"));
	}

	if (PrimaryWeaponData2 == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("실패 원인: PrimaryWeaponData가 비어있음! 에디터에서 할당했는지 확인하세요."));
	}

	if (EquippableComponent && PrimaryWeaponData2)
	{
		EquippableComponent->EquipWeapon(PrimaryWeaponData2);
		UE_LOG(LogTemp, Warning, TEXT("장착 성공! 서버 함수 호출함."));
	}
}

void ACH4Character::Server_ApplyItemEffect_Implementation(AHealItem* HealItem)
{
	ApplyItemEffect(HealItem);
	HealLog_Implementation();
}

void ACH4Character::ApplyItemEffect(AHealItem* HealItem)
{

	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("야후: 여기는 클라라서 못 지나감!"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("힐 되는중? 갯수까이나?"));
	if (HealItemCount <= 0 || !HealItem) return;

	CurrentHP = FMath::Clamp(CurrentHP + HealItem->HealAmount, 0.0f, MaxHP);
	HealItemCount--;

	UE_LOG(LogTemp, Log, TEXT("야후: 힐 완료! 남은 개수: %d"), HealItemCount);

	if (HealItemCount <= 0)
	{
		HealItem->Destroy();
		Heal = nullptr;
	}
}

void ACH4Character::HealLog_Implementation()
{

	UE_LOG(LogTemp, Warning, TEXT("힐 추가"));
}

void ACH4Character::OnApplyItemEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("야후: 4번 키 눌림!"));
	if (Heal != nullptr)
	{
		Server_ApplyItemEffect(Heal);
	}
}

void ACH4Character::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	AHealItem* LyingItem = Cast<AHealItem>(OtherActor);

	if (LyingItem)
	{
		Heal = LyingItem;
		HealItemCount++;

		LyingItem->SetActorHiddenInGame(true);
		LyingItem->SetActorEnableCollision(false);

		UE_LOG(LogTemp, Log, TEXT("야후: 아이템 획득! 현재 개수: %d"), HealItemCount);
	}

	AGearItem* GearItem = Cast<AGearItem>(OtherActor);
	if (GearItem)
	{
		ACH4GameState* Gs = Cast<ACH4GameState>(GamsState);
		Gs->AddGearPartsCount();
		GearItem->Destroy();
		UE_LOG(LogTemp, Error, TEXT("기어 파츠캣수:%d"), Gs->GearPartsCount);
	}
}

void ACH4Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACH4Character::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4Character::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACH4Character::Jump);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACH4Character::Fires);
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ACH4Character::OnEquipInput1);
	EnhancedInputComponent->BindAction(EquipAction2, ETriggerEvent::Started, this, &ACH4Character::OnEquipInput2);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACH4Character::StartSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACH4Character::StopSprint);
	
	EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Started, this, &ACH4Character::StartFreeLook);
	EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &ACH4Character::StopFreeLook);

	EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Started, this, &ACH4Character::OnApplyItemEffect);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ACH4Character::OnReload);

	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACH4Character::StartAim);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACH4Character::StopAim);
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACH4Character::Interact);
}

//테이크데메지
float ACH4Character::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Hit);

	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CurrentHP = CurrentHP = FMath::Clamp(CurrentHP - Damage, 0.0f, MaxHP);
	if (CurrentHP <= 0)
	{
		if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Down);
		ACH4PlayerState* PS = GetPlayerState<ACH4PlayerState>();
		if (PS->PlayerReviveCount <= 0)
		{
			OnDeath();
		}
	}
	return Damage;
}

//온데스
void ACH4Character::OnDeath()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Dead);
}

//플레이어 메쉬
void ACH4Character::InitializationPlayerMesh()
{
	//메쉬
	ConstructorHelpers::FObjectFinder<USkeletalMesh>
		PlayerSkeletalMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/AssetsvilleTown/Meshes/Characters/SK_citizen_male_01.SK_citizen_male_01'"));
	if (PlayerSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh.Object);
		GetMesh()->SetWorldLocationAndRotation(FVector(0, 0, -90), FRotator(0, 0, -90));//캡슐위치
	}
}

//플레이어 카메라
void ACH4Character::InitializationCamera()
{
	bUseControllerRotationYaw = false;

	//스프링암
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (SpringArm)
	{
		SpringArm->SetupAttachment(RootComponent);

		SpringArm->TargetArmLength = DefaultArmLength;
		SpringArm->TargetOffset = FVector(0.0f, 0.0f, 70.0f);
		SpringArm->SocketOffset = DefaultSocketOffset;
		SpringArm->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));

		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bDoCollisionTest = true;
	}
	//카메라
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if (Camera)
	{
		Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
		Camera->bUsePawnControlRotation = false;
		Camera->FieldOfView = DefaultFOV;
	}
}

//프리룩 사용
void ACH4Character::StartFreeLook()
{
	// 조준 중에는 자유시점 막기
	if (bIsAiming) return;

	bIsFreeLook = true;
	UpdateRotationMode();
}

//프리룩 종료
void ACH4Character::StopFreeLook()
{
	bIsFreeLook = false;
	UpdateRotationMode();
}


//인풋 
void ACH4Character::InitializationInput()
{
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Player/Input/IMC_Player.IMC_Player'"));
	if (InputContext.Object != nullptr)
	{
		DefaultContext = InputContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputMove(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Move.IA_Move'"));
	if (InputMove.Object != nullptr)
	{
		MoveAction = InputMove.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputLook(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Look.IA_Look'"));
	if (InputLook.Object != nullptr)
	{
		LookAction = InputLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputJump(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Jump.IA_Jump'"));
	if (InputJump.Object != nullptr)
	{
		JumpAction = InputJump.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputSprint(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Sprint.IA_Sprint'"));
	if (InputSprint.Object != nullptr)
	{
		SprintAction = InputSprint.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputFire(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Fire.IA_Fire'"));
	if (InputFire.Object != nullptr)
	{
		FireAction = InputFire.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputAim(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Aim.IA_Aim'"));
	if (InputAim.Object != nullptr)
	{
		AimAction = InputAim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputFreeLook(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_FreeLook.IA_FreeLook'"));
	if (InputFreeLook.Object != nullptr)
	{
		FreeLookAction = InputFreeLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputEquip(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Equip.IA_Equip'"));
	if (InputEquip.Object != nullptr)
	{
		EquipAction = InputEquip.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputEquip2(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Equip2.IA_Equip2'"));
	if (InputEquip.Object != nullptr)
	{
		EquipAction2 = InputEquip2.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputHeal(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Heal.IA_Heal'"));
	if (InputEquip.Object != nullptr)
	{
		HealAction = InputHeal.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputReload(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Reload.IA_Reload'"));
	if (InputEquip.Object != nullptr)
	{
		ReloadAction = InputReload.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputInteract(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Interaction.IA_Interaction'"));
	if (InputInteract.Object != nullptr)
	{
		InteractAction = InputInteract.Object;
	}

}

//무브
void ACH4Character::Move(const FInputActionValue& Value)
	{
	const FVector2D Movement = Value.Get<FVector2D>();

	FRotator MoveBasisRotation = FRotator::ZeroRotator;

	if (bIsFreeLook)
	{
		//프리룩 중에는 캐릭터가 바라보는 방향 기준으로 이동
		MoveBasisRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
	}
	else
	{
		//평소에는 카메라 방향 기준으로 이동
		const FRotator ControlRot = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;
		MoveBasisRotation = FRotator(0.0f, ControlRot.Yaw, 0.0f);
	}

	const FVector Forward = UKismetMathLibrary::GetForwardVector(MoveBasisRotation);
	const FVector Right = UKismetMathLibrary::GetRightVector(MoveBasisRotation);

	AddMovementInput(Forward, Movement.Y);
	AddMovementInput(Right, Movement.X);
}
//뛰기시작
void ACH4Character::StartSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

//뛰기 멈출때
void ACH4Character::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

//시점
void ACH4Character::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X * GetWorld()->DeltaTimeSeconds * mouseSpeed);
	AddControllerPitchInput(LookAxisVector.Y * GetWorld()->DeltaTimeSeconds * mouseSpeed);
}

//정조준 시작
void ACH4Character::StartAim()
{
	bIsFreeLook = false;
	bIsAiming = true;

	if (bIsSprinting)
	{
		StopSprint();//뛰는 도중 정조준하면 뛰는거 멈춤
	}
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;//이동속도 줄이기
	UpdateRotationMode();//회전 방식 바꾸는 함수
}

//정조준 멈춤
void ACH4Character::StopAim()
{
	bIsAiming = false;

	//뛰는 도중이면 뛰는 속도 유지, 아니면 보통 걸음 속도
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
	UpdateRotationMode();//일반 회전 방식으로 바뀜
}

//평소에는 이동방향에 따라 회전, 조준중에 카메라 방향따라 회전 전환하는 함수
void ACH4Character::UpdateRotationMode()
{
	if (bIsAiming)
	{
		//조준
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else if (bIsFreeLook)
	{
		//자유시점
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		//기본 상태
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

//카메라 업데이트 (정조준시)카메라 움직이기 위해서
void ACH4Character::UpdateAimCamera(float DeltaTime)
{
	if (!SpringArm || !Camera) return;

	const float TargetArmLength = bIsAiming ? AimArmLength : DefaultArmLength;//조준하고있나? 안하고있나 판단
	const FVector TargetSocketOffset = bIsAiming ? AimSocketOffset : DefaultSocketOffset;//카메라 오프셋 설정 조준상황에따라서
	const float TargetFOV = bIsAiming ? AimFOV : DefaultFOV;//시야각 설정

	//카메라 거리바뀔때 부드럽게 바뀌게 
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,//현재카메라 거리
		TargetArmLength,//목표 카메라 거리
		DeltaTime,//프레임 시간
		AimInterpSpeed//카메라 목표 도달 시간
	);

	//카메라를 거리를 줄이기도하고 옆으로 더 살짝 옮기게 
	SpringArm->SocketOffset = FMath::VInterpTo(
		SpringArm->SocketOffset,
		TargetSocketOffset,
		DeltaTime,
		AimInterpSpeed
	);
	
	//시야각도 부드럽게 바꿈
	Camera->FieldOfView = FMath::FInterpTo(
		Camera->FieldOfView,
		TargetFOV,
		DeltaTime,
		AimInterpSpeed
	);
}

//발사
void ACH4Character::Fires()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon: %s"),
		EquippableComponent->CurrentWeapon ? TEXT("있음") : TEXT("없음"));
	if (EquippableComponent == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, TEXT("컴포넌트 자체가 널입니다!"));
		return;
	}

	if (EquippableComponent->CurrentWeapon == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("컴포넌트는 있는데 총이 없습니다!"));
		return;
	}
	EquippableComponent->Fire();
}

void ACH4Character::OnReload()
{
	EquippableComponent->Reload();
}

// 몽타주 호출 함수들
void ACH4Character::PlayHitAnimation()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Hit);
}

void ACH4Character::PlayPickupAnimation()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Pickup);
}

void ACH4Character::PlayDownAnimation()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Down);
}

void ACH4Character::PlayDeathAnimation()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Dead);
}

void ACH4Character::PlayReviveAnimation()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Revive);
}

//멀티캐스트 구현
void ACH4Character::Multi_PlayAction_Implementation(EPlayerActionState NewState)
{
	UPlayerAnimInstance* AnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		switch (NewState)
		{
		case EPlayerActionState::Hit:    PlayAnimMontage(AnimInst->HitHeadMontage); break;
		case EPlayerActionState::Pickup: AnimInst->Montage_Play(AnimInst->PickupMontage); break;
		case EPlayerActionState::Down:   AnimInst->Montage_Play(AnimInst->DownMontage);   break;
		case EPlayerActionState::Dead:   AnimInst->Montage_Play(AnimInst->DeathMontage);  break;
		case EPlayerActionState::Revive: AnimInst->Montage_Play(AnimInst->ReviveMontage); break;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%d"), (int32)NewState);
}

//상호작용(소생>줍기)->우선순위
void ACH4Character::Interact()
{
	if (!HasAuthority())
	{
		Server_Interact();
	}
	else
	{
		Server_Interact_Implementation();
	}
}


void ACH4Character::Server_Interact_Implementation()
{
	if (TryReviveNearbyPlayer()) return;
	if (TryPickupNearbyItem()) return;
}


bool ACH4Character::TryReviveNearbyPlayer()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Revive);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		InteractionRadius,
		ObjectTypes,
		ACH4Character::StaticClass(),
		IgnoreActors,
		OutActors
	);

	if (!bHit)
	{
		return false;
	}

	for (AActor* Actor : OutActors)
	{
		ACH4Character* OtherCharacter = Cast<ACH4Character>(Actor);
		if (OtherCharacter == nullptr)
		{
			continue;
		}

		//다운된 플레이어를 태그로 구분 다운된 플레이어 BP 또는 캐릭터에 "DownedPlayer" 태그를 넣어두면 됨
		if (OtherCharacter->ActorHasTag(TEXT("DownedPlayer")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s 를 소생합니다."), *OtherCharacter->GetName());

			//다운된 플레이어의 부활 애니메이션 재생
			OtherCharacter->PlayReviveAnimation();

			//PlayPickupAnimation();(일부러막음)

			return true;
		}
	}

	return false;
}

bool ACH4Character::TryPickupNearbyItem()
{
	if (HasAuthority()) Multi_PlayAction(EPlayerActionState::Pickup);
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		InteractionRadius,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		OutActors
	);

	if (!bHit)
	{
		return false;
	}

	for (AActor* Actor : OutActors)
	{
		if (Actor == nullptr)
		{
			continue;
		}

		//줍기 가능한 아이템을 태그로 구분 아이템 BP 또는 액터에 "PickupItem" 태그를 넣어두면 됨
		if (Actor->ActorHasTag(TEXT("PickupItem")))
		{
			UE_LOG(LogTemp, Warning, TEXT("%s 아이템을 줍습니다."), *Actor->GetName());

			PlayPickupAnimation();

			// 실제 인벤토리 처리나 Destroy는
			// Actor->Destroy();

			return true;
		}
	}

	return false;
}
