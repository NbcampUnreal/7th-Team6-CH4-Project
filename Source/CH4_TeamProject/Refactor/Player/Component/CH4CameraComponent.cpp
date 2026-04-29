#include "CH4CameraComponent.h"

#include "CH4MovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

UCH4CameraComponent::UCH4CameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UCH4CameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateAimCamera(DeltaTime);
}

void UCH4CameraComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCH4CameraComponent, bIsAiming);
	DOREPLIFETIME(UCH4CameraComponent, bIsFreeLook);
}

void UCH4CameraComponent::RegisterCameraComponents(USpringArmComponent* InSpringArm, UCameraComponent* InCamera)
{
	SpringArm = InSpringArm;
	Camera    = InCamera;

	if (SpringArm)
	{
		SpringArm->bDoCollisionTest        = true;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->TargetArmLength         = DefaultArmLength;
		SpringArm->SocketOffset            = DefaultSocketOffset;
		SpringArm->TargetOffset            = FVector(0.0f, 0.0f, 70.0f);
		SpringArm->SetRelativeRotation(FRotator(-10.0f, 0.0f, 0.0f));
	}

	if (Camera)
	{
		Camera->bUsePawnControlRotation = false;
		Camera->FieldOfView = DefaultFOV;
	}

	if (ACharacter* OwnerChar = GetOwnerCharacter())
	{
		OwnerChar->bUseControllerRotationYaw = false;
	}
}

void UCH4CameraComponent::StartAim()
{
	bIsFreeLook = false;
	bIsAiming = true;

	if (UCH4MovementComponent* MoveComp = FindMovementComponent())
	{
		MoveComp->SetAimWalk(true);
	}

	UpdateRotationMode();
}

void UCH4CameraComponent::StopAim()
{
	bIsAiming = false;

	if (UCH4MovementComponent* MoveComp = FindMovementComponent())
	{
		MoveComp->SetAimWalk(false);
	}

	UpdateRotationMode();
}

void UCH4CameraComponent::StartFreeLook()
{
	if (bIsAiming) return;

	bIsFreeLook = true;
	UpdateRotationMode();
}

void UCH4CameraComponent::StopFreeLook()
{
	bIsFreeLook = false;
	UpdateRotationMode();
}

void UCH4CameraComponent::UpdateRotationMode() const
{
	ACharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return;

	UCharacterMovementComponent* Movement = OwnerChar->GetCharacterMovement();
	if (!Movement) return;

	if (bIsAiming)
	{
		OwnerChar->bUseControllerRotationYaw = true;
		Movement->bOrientRotationToMovement = false;
	}
	else if (bIsFreeLook)
	{
		OwnerChar->bUseControllerRotationYaw = false;
		Movement->bOrientRotationToMovement = false;
	}
	else
	{
		OwnerChar->bUseControllerRotationYaw = true;
		Movement->bOrientRotationToMovement = false;
	}
}

void UCH4CameraComponent::UpdateAimCamera(float DeltaTime) const
{
	if (!SpringArm || !Camera) return;

	const float   TargetArmLength    = bIsAiming ? AimArmLength    : DefaultArmLength;
	const FVector TargetSocketOffset = bIsAiming ? AimSocketOffset : DefaultSocketOffset;
	const float   TargetFOV          = bIsAiming ? AimFOV          : DefaultFOV;

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetArmLength,    DeltaTime, AimInterpSpeed);
	SpringArm->SocketOffset    = FMath::VInterpTo(SpringArm->SocketOffset,    TargetSocketOffset, DeltaTime, AimInterpSpeed);
	Camera->FieldOfView        = FMath::FInterpTo(Camera->FieldOfView,        TargetFOV,          DeltaTime, AimInterpSpeed);
}

ACharacter* UCH4CameraComponent::GetOwnerCharacter() const
{
	return Cast<ACharacter>(GetOwner());
}

UCH4MovementComponent* UCH4CameraComponent::FindMovementComponent() const
{
	if (ACharacter* OwnerChar = GetOwnerCharacter())
		return Cast<UCH4MovementComponent>(OwnerChar->GetCharacterMovement());
	
	return nullptr;
}
