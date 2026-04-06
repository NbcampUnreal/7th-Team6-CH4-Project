#include "CH4Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

ACH4Character::ACH4Character()
{

	PrimaryActorTick.bCanEverTick = true;

	InitializationPlayerMesh();//메쉬 함수 호출
	InitializationCamera();//카메라 함수 호출
	InitializationInput();//인풋 함수
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

	GetCharacterMovement()->MaxWalkSpeed = PlayerMoveSpeed;
}

void ACH4Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACH4Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACH4Character::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACH4Character::Look);
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
