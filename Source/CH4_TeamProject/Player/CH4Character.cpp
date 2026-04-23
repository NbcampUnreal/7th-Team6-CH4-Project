#include "CH4Character.h"

#include "CH4PlayerController.h"
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
#include "CH4_TeamProject/Player/PlayerAnimInstance.h"
#include "Animation/AnimInstance.h"
#include "CH4_TeamProject/Game/CH4GameState.h"
#include "CH4_TeamProject/Item/Consumable/ConsumableDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "../Item/Equippable/Ranged Weapon/RangedWeaponDataAsset.h"
#include "CH4_TeamProject/Item/Equippable/Equippable.h"
#include "CH4_TeamProject/Item/ThorwbleItem/ThorwbleItems.h"
#include "Components/CapsuleComponent.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "CH4_TeamProject/Item/Equippable/Equippable.h"
#include "CH4_TeamProject/Item/Equippable/Ranged Weapon/RangedWeapons.h"


ACH4Character::ACH4Character()
{
	PrimaryActorTick.bCanEverTick = true;

	InitializationPlayerMesh(); 
	InitializationCamera(); 
	InitializationInput(); 

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(
		TEXT("/Game/Player/PlayerBluePrint/ABP_Player.ABP_Player_C"));
	if (AnimInstance.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	}

	EquippableComponent = CreateDefaultSubobject<UEquippableComponent>(TEXT("EquippableComponent"));
	EquippableComponent->SetIsReplicated(true);
	bReplicates = true;
}


void ACH4Character::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultContext, 0);
		}
	}
	if (IsLocallyControlled())
	{
		
	}
	else
	{
		
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

	UpdateAimCamera(DeltaTime);
}

//포즈 상태 업데이트 
void ACH4Character::Server_UpdateCombatPose_Implementation()
{
	if (!EquippableComponent || !EquippableComponent->CurrentWeapon)
	{
		CurrentCombatPose = ECombatPose::Normal;
		return;
	}
	
	ECombatPose Data = EquippableComponent->CurrentWeapon->WeaponData->GetGunDataAsset();
	CurrentCombatPose = Data;
	UpdateCombatPose();
}

void ACH4Character::UpdateCombatPose_Implementation()
{
	
	OnRep_CombatPose();
}

void ACH4Character::OnRep_CombatPose()
{
	
	
}

void ACH4Character::OnEquipInput1()
{
	if (EquippableComponent && PrimaryWeaponData1)
	{
		EquippableComponent->Server_EquipWeapon(PrimaryWeaponData1);
		
		Server_UpdateCombatPose();
	}
}

void ACH4Character::OnEquipInput2()
{
	if (EquippableComponent && PrimaryWeaponData2)
	{
		EquippableComponent->Server_EquipWeapon(PrimaryWeaponData2);
		Server_UpdateCombatPose();
	}
}

void ACH4Character::OnEquipInput3()
{
	if (EquippableComponent && PrimaryWeaponData3)
	{
		EquippableComponent->Server_EquipWeapon(PrimaryWeaponData3);
		Server_UpdateCombatPose();
	}
}

void ACH4Character::OnEquipInput4()
{
	if (EquippableComponent && PrimaryWeaponData4)
	{
		EquippableComponent->Server_EquipWeapon(PrimaryWeaponData4);
		Server_UpdateCombatPose();
	}
}


void ACH4Character::Server_ApplyItemEffect_Implementation(AHealItem* HealItem)
{
	HealLog_Implementation();
}


void ACH4Character::HealLog_Implementation()
{
	
}

void ACH4Character::OnApplyItemEffect()
{
	
	if (HealItemCount > 0)
	{
		Server_UseHealItem();
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
	EnhancedInputComponent->BindAction(EquipAction3, ETriggerEvent::Started, this, &ACH4Character::OnEquipInput3);
	EnhancedInputComponent->BindAction(EquipAction4, ETriggerEvent::Started, this, &ACH4Character::OnEquipInput4);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACH4Character::StartSprint);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACH4Character::StopSprint);

	EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Started, this, &ACH4Character::StartFreeLook);
	EnhancedInputComponent->BindAction(FreeLookAction, ETriggerEvent::Completed, this, &ACH4Character::StopFreeLook);

	EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Started, this, &ACH4Character::OnApplyItemEffect);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ACH4Character::OnReload);
	EnhancedInputComponent->BindAction(ThrowAction, ETriggerEvent::Started, this, &ACH4Character::OnThrowGrenade);

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

	if (CurrentHP <= 0 && !bIsDowned) //이미 다운된 상태가 아닐 때만
	{
		if (HasAuthority()) //서버에서만 실행
		{
			bIsDowned = true; //기절 변수

			//몽타주 재생
			Multi_PlayAction(EPlayerActionState::Down);

			//이동 속도 0으로
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			GetCharacterMovement()->DisableMovement();
			ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetController());
			if (PC)
			{
				// 클라이언트에게 입력 중단 명령 (RPC)
				PC->Client_DisablePlayerInput();
				// 클라이언트에게 다운 UI 표시 명령 (RPC)
				PC->Client_SetPlayerDownedUI(true);
			}
		}

		ACH4PlayerState* PS = GetPlayerState<ACH4PlayerState>();
		if (PS && PS->PlayerReviveCount <= 0)
		{
			OnDeath();
		}
	}
	return Damage;
}

//플레이어 다운!
void ACH4Character::OnRep_IsDowned()
{
	if (bIsDowned)
	{
		//클라이언트에서도 속도를 0으로 맞춤
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
		}
	}
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
		PlayerSkeletalMesh(TEXT(
			"/Script/Engine.SkeletalMesh'/Game/Asset/AssetsvilleTown/Meshes/Characters/SK_citizen_male_01.SK_citizen_male_01'"));
	if (PlayerSkeletalMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh.Object);
		GetMesh()->SetWorldLocationAndRotation(FVector(0, 0, -90), FRotator(0, 0, -90)); //캡슐위치
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
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Player/Input/IMC_Player.IMC_Player'"));
	if (InputContext.Object != nullptr)
	{
		DefaultContext = InputContext.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputMove(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Move.IA_Move'"));
	if (InputMove.Object != nullptr)
	{
		MoveAction = InputMove.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputLook(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Look.IA_Look'"));
	if (InputLook.Object != nullptr)
	{
		LookAction = InputLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputJump(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Jump.IA_Jump'"));
	if (InputJump.Object != nullptr)
	{
		JumpAction = InputJump.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputSprint(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Sprint.IA_Sprint'"));
	if (InputSprint.Object != nullptr)
	{
		SprintAction = InputSprint.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputFire(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Fire.IA_Fire'"));
	if (InputFire.Object != nullptr)
	{
		FireAction = InputFire.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputAim(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Aim.IA_Aim'"));
	if (InputAim.Object != nullptr)
	{
		AimAction = InputAim.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputFreeLook(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_FreeLook.IA_FreeLook'"));
	if (InputFreeLook.Object != nullptr)
	{
		FreeLookAction = InputFreeLook.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputEquip(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Equip.IA_Equip'"));
	if (InputEquip.Object != nullptr)
	{
		EquipAction = InputEquip.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputEquip2(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Equip2.IA_Equip2'"));
	if (InputEquip.Object != nullptr)
	{
		EquipAction2 = InputEquip2.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputEquip3(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Equip3.IA_Equip3'"));
	if (InputEquip.Object != nullptr)
	{
		EquipAction3 = InputEquip3.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputEquip4(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Equip4.IA_Equip4'"));
	if (InputEquip.Object != nullptr)
	{
		EquipAction4 = InputEquip4.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputHeal(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Heal.IA_Heal'"));
	if (InputEquip.Object != nullptr)
	{
		HealAction = InputHeal.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputReload(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Reload.IA_Reload'"));
	if (InputEquip.Object != nullptr)
	{
		ReloadAction = InputReload.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputInteract(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Interaction.IA_Interaction'"));
	if (InputInteract.Object != nullptr)
	{
		InteractAction = InputInteract.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputThrow(
		TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Throw.IA_Throw'"));
	if (InputThrow.Object != nullptr)
	{
		ThrowAction = InputThrow.Object;
	}
}

//무브
void ACH4Character::Move(const FInputActionValue& Value)
{
	if (bIsDowned) return;

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
	if (bIsDowned) return;

	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	if (HasAuthority())
	{
		return;
	}

	Server_SetSprinting(bIsSprinting);
}

//뛰기 멈출때
void ACH4Character::StopSprint()
{


	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (HasAuthority())
	{
		return;
	}

	Server_SetSprinting(bIsSprinting);
}

void ACH4Character::Server_SetSprinting_Implementation(bool bSprint)
{
	bIsSprinting = bSprint;
	GetCharacterMovement()->MaxWalkSpeed = bSprint ? SprintSpeed : WalkSpeed;
}

void ACH4Character::OnRep_IsSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void ACH4Character::PlayerInputStart()
{
	UE_LOG(LogTemp, Error, TEXT("Timer Called"));
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetController());
	if (PC)
	{
		PC->ResetIgnoreMoveInput();
		PC->ResetIgnoreLookInput();
	}

	GetWorld()->GetTimerManager().ClearTimer(PickupMontageTimerHandle);
}

void ACH4Character::PlayerInputStop()
{
	ACH4PlayerController* PC = Cast<ACH4PlayerController>(GetController());
	if (PC)
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}
}

//시점
void ACH4Character::Look(const FInputActionValue& Value)
{
	if (bIsDowned) return;

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X * GetWorld()->DeltaTimeSeconds * mouseSpeed);
	AddControllerPitchInput(LookAxisVector.Y * GetWorld()->DeltaTimeSeconds * mouseSpeed);
}

//정조준 시작
void ACH4Character::StartAim()
{
	if (bIsDowned) return;
	bIsFreeLook = false;
	bIsAiming = true;

	if (bIsSprinting)
	{
		StopSprint(); //뛰는 도중 정조준하면 뛰는거 멈춤
	}
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed; //이동속도 줄이기
	UpdateRotationMode(); //회전 방식 바꾸는 함수
}

//정조준 멈춤
void ACH4Character::StopAim()
{
	bIsAiming = false;

	//뛰는 도중이면 뛰는 속도 유지, 아니면 보통 걸음 속도
	GetCharacterMovement()->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
	UpdateRotationMode(); //일반 회전 방식으로 바뀜
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

	const float TargetArmLength = bIsAiming ? AimArmLength : DefaultArmLength; //조준하고있나? 안하고있나 판단
	const FVector TargetSocketOffset = bIsAiming ? AimSocketOffset : DefaultSocketOffset; //카메라 오프셋 설정 조준상황에따라서
	const float TargetFOV = bIsAiming ? AimFOV : DefaultFOV; //시야각 설정

	//카메라 거리바뀔때 부드럽게 바뀌게 
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength, //현재카메라 거리
		TargetArmLength, //목표 카메라 거리
		DeltaTime, //프레임 시간
		AimInterpSpeed //카메라 목표 도달 시간
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
	// 컴포넌트가 없으면 종료
	if (EquippableComponent == nullptr)
	{
		
		return;
	}

	if (bIsDowned) return;

	//현재 무기가 없으면 종료
	if (EquippableComponent->CurrentWeapon == nullptr)
	{
		
		return;
	}

	//장전 중이면 발사 막기
	if (IsReloading())
	{
		
		return;
	}
	ARangedWeapons* Firearm = Cast<ARangedWeapons>(EquippableComponent->CurrentWeapon);
	if (Firearm && Firearm->CurrentAmmo <= 0) return;
	
	
	//현재 전투 포즈 확인용 로그
	
	//실제 발사
	EquippableComponent->Fire();
	
	if (!HasAuthority())
	{
		Server_FireMontage();
		return;
	}
	FireMontagePose();
	//현재 무기 포즈에 따라 발사 몽타주 실행
}

void ACH4Character::OnReload()
{
	


	if (bIsDowned) return;

	//컴포넌트 또는 현재 무기가 없으면 종료
	if (EquippableComponent == nullptr || EquippableComponent->CurrentWeapon == nullptr)
	{
		
		return;
	}


	int32 MaxAmmo = EquippableComponent->CurrentWeapon->WeaponData->GetMaxAmmo();

	int32 CurrentAmmo = EquippableComponent->CurrentWeapon->CurrentAmmo;


	if (CurrentAmmo >= MaxAmmo)return;
	if (EquippableComponent->CurrentWeapon->MaxClip <= 0) return;


	//이미 장전 중이면 다시 장전 막기
	if (IsReloading())
	{
		
		return;
	}

	//현재 전투 포즈 확인용 로그
	
	if (!HasAuthority())
	{
		Server_ReloadMontage();
	}
	ReloadMontage();
	EquippableComponent->Reload();
}

bool ACH4Character::IsReloading() const
{
	UPlayerAnimInstance* Anim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (!Anim) return false;

	EPlayerActionState State = Anim->GetCurrentActionState();

	return (State == EPlayerActionState::PistolReload ||
		State == EPlayerActionState::RifleReload);
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
		case EPlayerActionState::Hit:
			PlayAnimMontage(AnimInst->HitHeadMontage);
			break;
		case EPlayerActionState::Pickup:
			if (AnimInst && AnimInst->PickupMontage)
			{
				float PlayLength = AnimInst->Montage_Play(AnimInst->PickupMontage);
				float Delay = FMath::Max(0.f, PlayLength - 0.3f);

				PlayerInputStop();

				GetWorld()->GetTimerManager().SetTimer(
					PickupMontageTimerHandle,
					this,
					&ACH4Character::PlayerInputStart,
					Delay,
					false
				);
			}
			break;
		case EPlayerActionState::Down:
			AnimInst->Montage_Play(AnimInst->DownMontage);
			break;
		case EPlayerActionState::Dead:
			AnimInst->Montage_Play(AnimInst->DeathMontage);

			PlayerInputStop();

			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			GetWorldTimerManager().SetTimer(
				PlayerDestroyTimerHandle,
				this,
				&ACH4Character::PlayerDestroy,
				2.0f,
				false
			);
			break;
		case EPlayerActionState::Revive:
			AnimInst->Montage_Play(AnimInst->ReviveMontage);
			break;

		case EPlayerActionState::PistolFire:
			AnimInst->PlayPistolFireMontage();
			break;

		case EPlayerActionState::RifleFire:
			AnimInst->PlayRifleFireMontage();
			break;

		case EPlayerActionState::PistolReload:
			AnimInst->PlayPistolReloadMontage();
			break;

		case EPlayerActionState::RifleReload:
			AnimInst->PlayRifleReloadMontage();
			break;

		case EPlayerActionState::MeleeAttack:
			AnimInst->PlayMeleeAttackMontage();
			break;

		case EPlayerActionState::DrinkHealItem:
			AnimInst->PlayDrinkHealItemMontage();

		case EPlayerActionState::ShotgunFire:
			AnimInst->PlayShotgunFireMontage();
			break;

		case EPlayerActionState::ShotgunReload:
			AnimInst->PlayShotgunReloadMontage();
			break;
		}
	}
	
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
			ABaseItem* ItemActor = Cast<ABaseItem>(Actor);
			if (ItemActor)
			{
				if (ItemActor->ItemData)
				{
					ApplyItemEffect(ItemActor->ItemData);
				}
				else
				{
					
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("%s 아이템을 줍습니다."), *Actor->GetName());

			PlayPickupAnimation();
			// 실제 인벤토리 처리나 Destroy는
			Actor->Destroy();
			return true;
		}
	}
	return false;
}


void ACH4Character::ApplyItemEffect(UConsumableDataAsset* Data)
{
	if (!Data)
	{
		return;
	}

	switch (Data->Type)
	{
	case EEffectType::Ammo:
		{
			TArray<AActor*> AttachedActors;
			GetAttachedActors(AttachedActors);

			for (AActor* Actor : AttachedActors)
			{
				if (AEquippable* FoundWeapon = Cast<AEquippable>(Actor))
				{
					FoundWeapon->AddMaxClip(Data->Value);

					return;
				}
			}
			
		}
		break;

	case EEffectType::Health:
		{
			HealItemCount++;
		}
		break;

	case EEffectType::Gear:
		{
			

			if (ACH4GameState* GS = GetWorld()->GetGameState<ACH4GameState>())
			{
				GS->AddGearPartsCount();
				
			}
			else
			{
				
			}
		}
		break;
	case EEffectType::Grenade:
		{
			GrenadeCount++;
			
		}

	default:
		
		break;
	}
}

void ACH4Character::Server_ThrowGrenade_Implementation()
{
	if (bUSingGrenade)
	{
		
		return;
	}

	if (GrenadeCount <= 0)
	{
		
		return;
	}

	if (!GrenadeClass)
	{
		
		return;
	}


	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("Weapon_r"));
	FRotator SpawnRotation = GetControlRotation();

	AThorwbleItems* Grenade = GetWorld()->SpawnActor<AThorwbleItems>(
		GrenadeClass, SpawnLocation, SpawnRotation);

	if (Grenade)
	{
		GrenadeCount--;
		Grenade->SetOwner(this);
		Grenade->SetInstigator(this);

		FVector ThrowVelocity = GetControlRotation().Vector() * 1500.0f;
		Grenade->ProjectileMovement->Velocity = ThrowVelocity;

		
		       *SpawnLocation.ToString(), *ThrowVelocity.ToString());
		GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, Grenade, &AThorwbleItems::Explode, 1.5f, false);
		bUSingGrenade = true;
	}
	else
	{
		    
	}
	GetWorld()->GetTimerManager().SetTimer(GrenadeTimer, this, &ACH4Character::CanUSingGrenade, 5.f, false);
}

void ACH4Character::ThrowGrenade()
{
	Server_ThrowGrenade();
}

void ACH4Character::OnThrowGrenade()
{
	
	ThrowGrenade();
}

void ACH4Character::CanUSingGrenade()
{
	bUSingGrenade = false;
}

void ACH4Character::FireMontagePose()
{
	switch (CurrentCombatPose)
	{
	case ECombatPose::Pistol:
		
		Multi_PlayAction(EPlayerActionState::PistolFire);
		break;

	case ECombatPose::Rifle:
	case ECombatPose::MuchineGun:
		
		Multi_PlayAction(EPlayerActionState::RifleFire);
		break;

	case ECombatPose::Shotgun:
		
		Multi_PlayAction(EPlayerActionState::ShotgunFire);
		break;

	default:
		
		break;
	}
}

void ACH4Character::ReloadMontage()
{
	//현재 무기 포즈에 따라 장전 몽타주 실행
	if (HasAuthority())
	{
		switch (CurrentCombatPose)
		{
		case ECombatPose::Pistol:
			
			Multi_PlayAction(EPlayerActionState::PistolReload);
			break;

		case ECombatPose::Rifle:
			
			Multi_PlayAction(EPlayerActionState::RifleReload);
			break;
		case ECombatPose::Shotgun:
			
			Multi_PlayAction(EPlayerActionState::ShotgunReload);
			break;

		default:
			
			break;
		}
	}
}



void ACH4Character::Server_ReloadMontage_Implementation()
{
	ReloadMontage();
}

void ACH4Character::Server_FireMontage_Implementation()
{
	FireMontagePose();
}

void ACH4Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4Character, CurrentCombatPose);
	DOREPLIFETIME(ACH4Character, HealItemCount);
	DOREPLIFETIME(ACH4Character, GrenadeCount);
	DOREPLIFETIME(ACH4Character, bUSingGrenade);
	DOREPLIFETIME(ACH4Character, bIsSprinting);
	DOREPLIFETIME(ACH4Character, bIsAiming);
	DOREPLIFETIME(ACH4Character, bIsFreeLook);
	DOREPLIFETIME(ACH4Character, MaxHP);
	DOREPLIFETIME(ACH4Character, WalkSpeed);
	DOREPLIFETIME(ACH4Character, SprintSpeed);
	DOREPLIFETIME(ACH4Character, CurrentHP);
	DOREPLIFETIME(ACH4Character, bIsDowned);
	DOREPLIFETIME(ACH4Character, PrimaryWeaponData1);
	DOREPLIFETIME(ACH4Character, PrimaryWeaponData2);
	DOREPLIFETIME(ACH4Character, PrimaryWeaponData3);
	DOREPLIFETIME(ACH4Character, PrimaryWeaponData4);
}

void ACH4Character::Server_UseHealItem_Implementation()
{
	if (bIsDowned) return;
	
	if (DefaultHealData && HealItemCount > 0)
	{
		CurrentHP = FMath::Clamp(CurrentHP + DefaultHealData->Value, 0.0f, MaxHP);
		HealItemCount--;
	}
	
}

void ACH4Character::PlayerDestroy()
{
	Destroy();
}
