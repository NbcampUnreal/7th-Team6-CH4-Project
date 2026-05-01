#include "CH4MeleeWeaponAttackNotify.h"

#include "../Weapon/CH4WeaponBase.h"
#include "CH4_TeamProject/Refactor/Player/Character/CH4PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Misc/OutputDeviceNull.h"

void UCH4MeleeWeaponAttackNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) return;

	const ACH4PlayerCharacter* Player = Cast<ACH4PlayerCharacter>(MeshComp->GetOwner());
	if (!Player || !Player->CurrentWeapon) return;

	FOutputDeviceNull Ar;
	Player->CurrentWeapon->CallFunctionByNameWithArguments(
		*FunctionName.ToString(), Ar, nullptr, true);
}
