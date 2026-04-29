#include "CH4MovementComponent.h"

#include "GameFramework/Character.h"

UCH4MovementComponent::UCH4MovementComponent()
{
	MaxWalkSpeed = DefaultWalkSpeed;
	bOrientRotationToMovement = true;
	RotationRate = FRotator(0.0f, 720.0f, 0.0f);
}

float UCH4MovementComponent::GetMaxSpeed() const
{
	if (bWantsAimWalk) return AimWalkSpeed;
	if (bWantsToSprint) return SprintSpeed;
	return Super::GetMaxSpeed();
}

void UCH4MovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bWantsAimWalk = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

FNetworkPredictionData_Client* UCH4MovementComponent::GetPredictionData_Client() const
{
	if (!ClientPredictionData)
	{
		UCH4MovementComponent* MutableThis = const_cast<UCH4MovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CH4(*this);
	}
	return ClientPredictionData;
}

void UCH4MovementComponent::StartSprint()
{
	bWantsToSprint = true;
}

void UCH4MovementComponent::StopSprint()
{
	bWantsToSprint = false;
}

void UCH4MovementComponent::SetAimWalk(bool bAim)
{
	bWantsAimWalk = bAim;
}

FSavedMove_CH4::FSavedMove_CH4()
	: bSavedWantsToSprint(0)
	, bSavedWantsAimWalk(0)
{
}

void FSavedMove_CH4::Clear()
{
	Super::Clear();
	bSavedWantsToSprint = 0;
	bSavedWantsAimWalk = 0;
}

uint8 FSavedMove_CH4::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (bSavedWantsToSprint) Result |= FLAG_Custom_0;
	if (bSavedWantsAimWalk) Result |= FLAG_Custom_1;
	return Result;
}

bool FSavedMove_CH4::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	const FSavedMove_CH4* NewCH4Move = static_cast<const FSavedMove_CH4*>(NewMove.Get());
	if (!NewCH4Move) return false;
	if (bSavedWantsToSprint != NewCH4Move->bSavedWantsToSprint) return false;
	if (bSavedWantsAimWalk != NewCH4Move->bSavedWantsAimWalk) return false;
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

void FSavedMove_CH4::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	if (UCH4MovementComponent* CMC = Cast<UCH4MovementComponent>(Character ? Character->GetCharacterMovement() : nullptr))
	{
		bSavedWantsToSprint = CMC->bWantsToSprint ? 1 : 0;
		bSavedWantsAimWalk = CMC->bWantsAimWalk ? 1 : 0;
	}
}

void FSavedMove_CH4::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	if (UCH4MovementComponent* CMC = Cast<UCH4MovementComponent>(Character ? Character->GetCharacterMovement() : nullptr))
	{
		CMC->bWantsToSprint = bSavedWantsToSprint != 0;
		CMC->bWantsAimWalk = bSavedWantsAimWalk != 0;
	}
}

FNetworkPredictionData_Client_CH4::FNetworkPredictionData_Client_CH4(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_CH4::AllocateNewMove()
{
	return MakeShared<FSavedMove_CH4>();
}
