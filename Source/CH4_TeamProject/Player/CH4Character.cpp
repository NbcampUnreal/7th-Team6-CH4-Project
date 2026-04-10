#include "CH4Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "CH4_TeamProject/Game/CH4PlayerState.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "../Item/Equippable/EquippableComponent.h"
#include "CH4_TeamProject/Item/Equippable/Ranged Weapon/RangedWeapons.h"
#include "../Type.h"
#include "CH4_TeamProject/Item/Consumable/ConsumableDataAsset.h"
#include "CH4_TeamProject/Item/Consumable/HealItem.h"
#include "../Item/Consumable/HealItem.h"
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
	
	GetCharacterMovement()->MaxWalkSpeed = PlayerMoveSpeed;
}

void ACH4Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACH4Character::OnEquipInput()
{
	UE_LOG(LogTemp, Error, TEXT("호출은 성공"));

	if (EquippableComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("실패 원인: EquippableComponent가 비어있음!"));
	}

	if (PrimaryWeaponData == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("실패 원인: PrimaryWeaponData가 비어있음! 에디터에서 할당했는지 확인하세요."));
	}

	if (EquippableComponent && PrimaryWeaponData)
	{
		EquippableComponent->EquipWeapon(PrimaryWeaponData);
		UE_LOG(LogTemp, Warning, TEXT("장착 성공! 서버 함수 호출함."));
	}
}

void ACH4Character::ApplyItemEffect(AHealItem* HealItem)
{
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

void ACH4Character::OnApplyItemEffect()
{
	UE_LOG(LogTemp, Warning, TEXT("야후: 4번 키 눌림!"));
	if (Heal != nullptr)
	{
		ApplyItemEffect(Heal);
	}
}

void ACH4Character::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	AHealItem* LyingItem = Cast<AHealItem>(OtherActor);
    
	if (LyingItem)
	{
		// 내 리모컨(Heal)에 이 아이템을 등록!
		Heal = LyingItem;
		HealItemCount++;

		// 바닥에서 안 보이게 숨기고 충돌 끄기 (인벤토리에 넣은 것처럼 처리)
		LyingItem->SetActorHiddenInGame(true);
		LyingItem->SetActorEnableCollision(false);

		UE_LOG(LogTemp, Log, TEXT("야후: 아이템 획득! 현재 개수: %d"), HealItemCount);
	}
}

void ACH4Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACH4Character::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4Character::Look);
	EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACH4Character::Fires);
	EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ACH4Character::OnEquipInput);
	EnhancedInputComponent->BindAction(HealAction, ETriggerEvent::Started, this, &ACH4Character::OnApplyItemEffect);
	
}

float ACH4Character::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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

		SpringArm->TargetArmLength = 470.f;
		SpringArm->TargetOffset = FVector(0.0f, 0.0f, 70.0f);
		SpringArm->SocketOffset = FVector(0.0f, 25.0f, 20.0f);
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
		Camera->FieldOfView = 80.0f;
	}
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
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputFire(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Fire.IA_Fire'"));
	if (InputFire.Object != nullptr)
	{
		FireAction = InputFire.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputEquip(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Equip.IA_Equip'"));
	if (InputEquip.Object != nullptr)
	{
		EquipAction = InputEquip.Object;
	}
	static ConstructorHelpers::FObjectFinder<UInputAction> InputHeal(TEXT("/Script/EnhancedInput.InputAction'/Game/Player/Input/Action/IA_Heal.IA_Heal'"));
	if (InputEquip.Object != nullptr)
	{
		HealAction = InputHeal.Object;
	}
	
}

//무브
void ACH4Character::Move(const FInputActionValue& Value)
{
	const FVector2D Movement = Value.Get<FVector2D>();

	const FRotator ControlRot = Controller ? Controller->GetControlRotation() : FRotator::ZeroRotator;
	const FRotator YawOnly(0.0f, ControlRot.Yaw, 0.0f);

	const FVector Forward = UKismetMathLibrary::GetForwardVector(YawOnly);
	const FVector Right = UKismetMathLibrary::GetRightVector(YawOnly);

	AddMovementInput(Forward, Movement.Y);
	AddMovementInput(Right, Movement.X);
}

//시점
void ACH4Character::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X * GetWorld()->DeltaTimeSeconds * mouseSpeed);
	AddControllerPitchInput(LookAxisVector.Y * GetWorld()->DeltaTimeSeconds * mouseSpeed);
}

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



