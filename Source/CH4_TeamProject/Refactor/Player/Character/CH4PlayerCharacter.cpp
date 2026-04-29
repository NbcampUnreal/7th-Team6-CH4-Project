#include "CH4PlayerCharacter.h"

#include "../Component/CH4CameraComponent.h"
#include "../Component/CH4SkillComponent.h"
#include "../Component/CH4HealthComponent.h"
#include "../Component/CH4InteractionComponent.h"
#include "../Component/CH4MovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "CH4_TeamProject/Refactor/Framework/GameState/CH4DefenseGameState.h"
#include "CH4_TeamProject/Refactor/Framework/PlayerState/CH4DefensePlayerState.h"
#include "CH4_TeamProject/Refactor/Player/Controller/CH4DefensePlayerController.h"
#include "CH4_TeamProject/Refactor/Player/Animation/CH4PlayerAnimInstance.h"
#include "CH4_TeamProject/Refactor/Weapon/CH4WeaponBase.h"
#include "CH4_TeamProject/Refactor/Weapon/Data/CH4WeaponData.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "CH4_TeamProject/Refactor/Common/CH4Macros.h"
#include "CH4_TeamProject/Refactor/Item/CH4ItemBase.h"

ACH4PlayerCharacter::ACH4PlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCH4MovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	if (SpringArm)
	{
		SpringArm->SetupAttachment(RootComponent);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bDoCollisionTest = true;
	}

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	if (Camera)
	{
		Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
		Camera->bUsePawnControlRotation = false;
	}

	CH4MovementComponent = Cast<UCH4MovementComponent>(GetCharacterMovement());
	CameraLogicComponent = CreateDefaultSubobject<UCH4CameraComponent>(TEXT("CameraLogicComponent"));
	HealthComponent      = CreateDefaultSubobject<UCH4HealthComponent>(TEXT("HealthComponent"));
	InteractionComponent = CreateDefaultSubobject<UCH4InteractionComponent>(TEXT("InteractionComponent"));
	SkillComponent       = CreateDefaultSubobject<UCH4SkillComponent>(TEXT("SkillComponent"));

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

	// BP 디폴트 슬롯 인덱스 안정용 고정 크기.
	PrimaryWeaponDatas.SetNum(MAX_WEAPON_SLOTS);

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> PlayerSkeletalMesh(
		TEXT("/Script/Engine.SkeletalMesh'/Game/Asset/AssetsvilleTown/Meshes/Characters/SK_citizen_male_01.SK_citizen_male_01'"));
	if (PlayerSkeletalMesh.Succeeded() && GetMesh())
	{
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh.Object);
		GetMesh()->SetWorldLocationAndRotation(FVector(0, 0, -90), FRotator(0, 0, -90));
	}

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstance(
		TEXT("/Game/Player/PlayerBluePrint/ABP_Player.ABP_Player_C"));
	if (AnimInstance.Succeeded() && GetMesh())
	{
		GetMesh()->SetAnimInstanceClass(AnimInstance.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputContext(
		TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Player/Input/IMC_Player.IMC_Player'"));
	if (InputContext.Succeeded()) DefaultContext = InputContext.Object;

#define INPUT_DIR "/Game/Player/Input/Action"
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Move,       INPUT_DIR, IA_Move)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Look,       INPUT_DIR, IA_Look)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Jump,       INPUT_DIR, IA_Jump)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Fire,       INPUT_DIR, IA_Fire)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Reload,     INPUT_DIR, IA_Reload)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Sprint,     INPUT_DIR, IA_Sprint)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Aim,        INPUT_DIR, IA_Aim)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::FreeLook,   INPUT_DIR, IA_FreeLook)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Interact,   INPUT_DIR, IA_Interaction)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Heal,       INPUT_DIR, IA_Heal)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::Throw,      INPUT_DIR, IA_Throw)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot1, INPUT_DIR, IA_Equip)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot2, INPUT_DIR, IA_Equip2)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot3, INPUT_DIR, IA_Equip3)
	LOAD_INPUT_ACTION_INTO_MAP(InputActions, ECH4PlayerInputAction::EquipSlot4, INPUT_DIR, IA_Equip4)
#undef INPUT_DIR
}

void ACH4PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACH4PlayerCharacter, CurrentCombatPose);
	DOREPLIFETIME(ACH4PlayerCharacter, CurrentWeapon);
	DOREPLIFETIME(ACH4PlayerCharacter, CurrentSlotIndex);
}

void ACH4PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Enhanced = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Enhanced) return;

	using EAction = ECH4PlayerInputAction;

	BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Move,       Triggered, HandleMoveInput);
	BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Look,       Triggered, HandleLookInput);
	BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Jump,       Triggered, Jump);
	BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Fire,       Triggered, Fires);
	BIND_MAPPED_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Reload,     Started,   OnReload);

	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::EquipSlot1, Started, Server_EquipWeaponSlot, 0);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::EquipSlot2, Started, Server_EquipWeaponSlot, 1);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::EquipSlot3, Started, Server_EquipWeaponSlot, 2);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::EquipSlot4, Started, Server_EquipWeaponSlot, 3);

	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Sprint,   Started,   HandleInputAction, EAction::Sprint,   true);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Sprint,   Completed, HandleInputAction, EAction::Sprint,   false);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Aim,      Started,   HandleInputAction, EAction::Aim,      true);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Aim,      Completed, HandleInputAction, EAction::Aim,      false);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::FreeLook, Started,   HandleInputAction, EAction::FreeLook, true);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::FreeLook, Completed, HandleInputAction, EAction::FreeLook, false);

	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Interact, Started, HandleInputAction, EAction::Interact, true);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Heal,     Started, HandleInputAction, EAction::Heal,     true);
	BIND_MAPPED_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, InputActions, EAction::Throw,    Started, HandleInputAction, EAction::Throw,    true);
}

float ACH4PlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HasAuthority()) Multicast_PlayAction(ECH4PlayerActionState::Hit);

	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent)
	{
		HealthComponent->ApplyDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	}

	return Damage;
}

void ACH4PlayerCharacter::Server_EquipWeaponSlot_Implementation(int32 SlotIndex)
{
	ACH4WeaponBase* Weapon = GetOrSpawnWeaponForSlot(SlotIndex);
	if (!Weapon) return;

	// 슬롯 swap 시 예비 탄약 풀(MaxClip)을 새 무기에 양도.
	if (CurrentWeapon)
	{
		SharedMaxClip = CurrentWeapon->MaxClip;
		CurrentWeapon->Unequip();
	}

	CurrentWeapon = Weapon;
	CurrentSlotIndex = SlotIndex;
	Weapon->MaxClip = SharedMaxClip;
	CurrentWeapon->Equip(this);

	Server_UpdateCombatPose();
}

void ACH4PlayerCharacter::Fires()
{
	if (!CurrentWeapon) return;
	if (HealthComponent && HealthComponent->IsDowned()) return;
	if (IsReloading()) return;

	CurrentWeapon->Server_Attack();
	CurrentWeapon->PlayWeaponShake();

	if (!HasAuthority())
	{
		Server_FireMontage();
		return;
	}
	FireMontagePose();
}

void ACH4PlayerCharacter::OnReload()
{
	if (!CurrentWeapon) return;
	if (HealthComponent && HealthComponent->IsDowned()) return;
	if (IsReloading()) return;

	if (!HasAuthority())
	{
		Server_ReloadMontage();
	}
	ReloadMontage();
	CurrentWeapon->Server_Reload();
}

bool ACH4PlayerCharacter::IsReloading() const
{
	const UCH4PlayerAnimInstance* Anim = Cast<UCH4PlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (!Anim) return false;

	const ECH4PlayerActionState State = Anim->GetCurrentActionState();
	return State == ECH4PlayerActionState::PistolReload || State == ECH4PlayerActionState::RifleReload;
}

void ACH4PlayerCharacter::OnRep_CombatPose()
{
}

void ACH4PlayerCharacter::Server_UpdateCombatPose_Implementation()
{
	if (!CurrentWeapon || !CurrentWeapon->WeaponDataAsset)
	{
		CurrentCombatPose = ECombatPose::Normal;
		return;
	}
	CurrentCombatPose = CurrentWeapon->WeaponDataAsset->GetCombatPose();
	Multicast_UpdateCombatPose();
}

void ACH4PlayerCharacter::Multicast_UpdateCombatPose_Implementation()
{
	OnRep_CombatPose();
}

void ACH4PlayerCharacter::Multicast_PlayAction_Implementation(ECH4PlayerActionState NewState)
{
	UCH4PlayerAnimInstance* AnimInst = Cast<UCH4PlayerAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInst)
	{
		AnimInst->PlayActionMontage(NewState);
	}

	switch (NewState)
	{
	case ECH4PlayerActionState::Pickup:
		// 픽업 몽타주 길이 - 0.3s 동안 입력 차단 후 복구.
		if (AnimInst)
		{
			const FCH4ActionMontageEntry* Entry = AnimInst->ActionMontages.Find(ECH4PlayerActionState::Pickup);
			if (Entry && Entry->Montage)
			{
				const float Delay = FMath::Max(0.f, Entry->Montage->GetPlayLength() - 0.3f);
				StopPlayerInput();
				GetWorldTimerManager().SetTimer(
					PickupMontageTimerHandle,
					this,
					&ACH4PlayerCharacter::StartPlayerInput,
					Delay,
					false);
			}
		}
		break;

	case ECH4PlayerActionState::Dead:
		StopPlayerInput();
		if (UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		GetWorldTimerManager().SetTimer(
			PlayerDestroyTimerHandle,
			this,
			&ACH4PlayerCharacter::PlayerDestroy,
			2.0f,
			false);
		break;

	default:
		break;
	}
}

void ACH4PlayerCharacter::PlayHitAnimation()    { if (HasAuthority()) Multicast_PlayAction(ECH4PlayerActionState::Hit); }
void ACH4PlayerCharacter::PlayPickupAnimation() { if (HasAuthority()) Multicast_PlayAction(ECH4PlayerActionState::Pickup); }
void ACH4PlayerCharacter::PlayDownAnimation()   { if (HasAuthority()) Multicast_PlayAction(ECH4PlayerActionState::Down); }
void ACH4PlayerCharacter::PlayDeathAnimation()  { if (HasAuthority()) Multicast_PlayAction(ECH4PlayerActionState::Dead); }
void ACH4PlayerCharacter::PlayReviveAnimation() { if (HasAuthority()) Multicast_PlayAction(ECH4PlayerActionState::Revive); }

void ACH4PlayerCharacter::Server_FireMontage_Implementation()
{
	FireMontagePose();
}

void ACH4PlayerCharacter::FireMontagePose()
{
	switch (CurrentCombatPose)
	{
	case ECombatPose::Pistol:
		Multicast_PlayAction(ECH4PlayerActionState::PistolFire);
		break;
	case ECombatPose::Rifle:
	case ECombatPose::MuchineGun:
		Multicast_PlayAction(ECH4PlayerActionState::RifleFire);
		break;
	case ECombatPose::Shotgun:
		Multicast_PlayAction(ECH4PlayerActionState::ShotgunFire);
		break;
	default:
		break;
	}
}

void ACH4PlayerCharacter::Server_ReloadMontage_Implementation()
{
	ReloadMontage();
}

void ACH4PlayerCharacter::ReloadMontage()
{
	if (!HasAuthority()) return;

	switch (CurrentCombatPose)
	{
	case ECombatPose::Pistol:
		Multicast_PlayAction(ECH4PlayerActionState::PistolReload);
		break;
	case ECombatPose::Rifle:
		Multicast_PlayAction(ECH4PlayerActionState::RifleReload);
		break;
	case ECombatPose::Shotgun:
		Multicast_PlayAction(ECH4PlayerActionState::ShotgunReload);
		break;
	default:
		break;
	}
}

void ACH4PlayerCharacter::PlayerDestroy()
{
	Destroy();
}

void ACH4PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			SubSystem->AddMappingContext(DefaultContext, 0);
		}
	}

	if (CameraLogicComponent)
	{
		CameraLogicComponent->RegisterCameraComponents(SpringArm, Camera);
	}

	GameState = Cast<ACH4DefenseGameState>(GetWorld()->GetGameState());

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

void ACH4PlayerCharacter::HandleMoveInput(const FInputActionValue& Value)
{
	if (HealthComponent && HealthComponent->IsDowned()) return;

	const FVector2D Movement = Value.Get<FVector2D>();
	const bool bFreeLook = CameraLogicComponent && CameraLogicComponent->IsFreeLook();

	FRotator MoveBasisRotation = FRotator::ZeroRotator;
	if (bFreeLook)
	{
		MoveBasisRotation = FRotator(0.0f, GetActorRotation().Yaw, 0.0f);
	}
	else
	{
		const FRotator ControlRot = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;
		MoveBasisRotation = FRotator(0.0f, ControlRot.Yaw, 0.0f);
	}

	const FVector Forward = MoveBasisRotation.Vector();
	const FVector Right = FRotationMatrix(MoveBasisRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Movement.Y);
	AddMovementInput(Right, Movement.X);
}

void ACH4PlayerCharacter::HandleLookInput(const FInputActionValue& Value)
{
	if (HealthComponent && HealthComponent->IsDowned()) return;

	const UWorld* World = GetWorld();
	if (!World) return;

	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X * World->DeltaTimeSeconds * MouseSensitivity);
	AddControllerPitchInput(LookAxisVector.Y * World->DeltaTimeSeconds * MouseSensitivity);
}

void ACH4PlayerCharacter::HandleInputAction(ECH4PlayerInputAction Action, bool bActive)
{
	switch (Action)
	{
	case ECH4PlayerInputAction::Sprint:
		{
			UCH4MovementComponent* MoveComp = Cast<UCH4MovementComponent>(GetCharacterMovement());
			if (!MoveComp) return;
			if (bActive)
			{
				if (HealthComponent && HealthComponent->IsDowned()) return;
				MoveComp->StartSprint();
			}
			else
			{
				MoveComp->StopSprint();
			}
		}
		break;

	case ECH4PlayerInputAction::Aim:
		if (CameraLogicComponent)
		{
			// Aim 진입 시 sprint 강제 해제.
			if (bActive && CH4MovementComponent)
			{
				CH4MovementComponent->StopSprint();
			}
			bActive ? CameraLogicComponent->StartAim() : CameraLogicComponent->StopAim();
		}
		break;

	case ECH4PlayerInputAction::FreeLook:
		if (CameraLogicComponent)
		{
			bActive ? CameraLogicComponent->StartFreeLook() : CameraLogicComponent->StopFreeLook();
		}
		break;

	case ECH4PlayerInputAction::Interact:
		if (InteractionComponent) InteractionComponent->Interact();
		break;

	case ECH4PlayerInputAction::Heal:
		if (SkillComponent) SkillComponent->OnApplyItemEffect();
		break;

	case ECH4PlayerInputAction::Throw:
		if (SkillComponent) SkillComponent->OnThrowGrenade();
		break;
	}
}

void ACH4PlayerCharacter::OnPickupRequestedFromInteraction()
{
	PlayPickupAnimation();
}

void ACH4PlayerCharacter::OnReviveRequestedFromInteraction(ACharacter* TargetCharacter)
{
	if (!HasAuthority()) return;

	ACH4PlayerCharacter* Target = Cast<ACH4PlayerCharacter>(TargetCharacter);
	if (!Target || !Target->HealthComponent) return;

	Target->HealthComponent->Revive(50.0f);
	Target->PlayReviveAnimation();
	Target->StartPlayerInput();

	if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(Target->GetController()))
	{
		PC->Client_SetPlayerDownedUI(false);
	}
}

void ACH4PlayerCharacter::OnPlayerDownedFromHealth()
{
	if (!HasAuthority()) return;

	Multicast_PlayAction(ECH4PlayerActionState::Down);

	if (ACH4DefensePlayerController* PC = Cast<ACH4DefensePlayerController>(GetController()))
	{
		PC->Client_DisablePlayerInput();
		PC->Client_SetPlayerDownedUI(true);
	}

	if (const ACH4DefensePlayerState* PS = GetPlayerState<ACH4DefensePlayerState>())
	{
		if (PS->PlayerReviveCount <= 0)
		{
			Multicast_PlayAction(ECH4PlayerActionState::Dead);
		}
	}
}

void ACH4PlayerCharacter::StopPlayerInput()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}
}

void ACH4PlayerCharacter::StartPlayerInput()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->ResetIgnoreMoveInput();
		PC->ResetIgnoreLookInput();
	}
	GetWorldTimerManager().ClearTimer(PickupMontageTimerHandle);
}

UCH4WeaponData* ACH4PlayerCharacter::GetWeaponDataForSlot(int32 SlotIndex) const
{
	return PrimaryWeaponDatas.IsValidIndex(SlotIndex) ? PrimaryWeaponDatas[SlotIndex] : nullptr;
}

ACH4WeaponBase* ACH4PlayerCharacter::GetOrSpawnWeaponForSlot(int32 SlotIndex)
{
	UCH4WeaponData* Data = GetWeaponDataForSlot(SlotIndex);
	if (!Data || !Data->WeaponClass) return nullptr;

	UWorld* World = GetWorld();
	if (!World) return nullptr;

	if (EquippedWeapons.Num() < MAX_WEAPON_SLOTS) EquippedWeapons.SetNum(MAX_WEAPON_SLOTS);

	if (EquippedWeapons[SlotIndex]) return EquippedWeapons[SlotIndex];

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;

	AActor* Spawned = World->SpawnActor<AActor>(Data->WeaponClass, GetActorTransform(), Params);
	ACH4WeaponBase* Weapon = Cast<ACH4WeaponBase>(Spawned);
	if (!Weapon)
	{
		if (Spawned) Spawned->Destroy();
		return nullptr;
	}

	Weapon->WeaponDataAsset = Data;
	// 호스트 본인 화면에서도 메쉬가 즉시 보이도록 OnRep 수동 호출 + 클라 즉시 복제.
	Weapon->OnRep_WeaponData();
	Weapon->ForceNetUpdate();
	Weapon->Unequip();
	EquippedWeapons[SlotIndex] = Weapon;
	return Weapon;
}
