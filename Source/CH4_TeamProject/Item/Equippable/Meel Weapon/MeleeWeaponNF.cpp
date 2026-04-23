// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeaponNF.h"

#include "CH4_TeamProject/Item/Equippable/Equippable.h"
#include "CH4_TeamProject/Item/Equippable/EquippableComponent.h"
#include "CH4_TeamProject/Player/CH4Character.h"
#include "Misc/OutputDeviceNull.h"


void UMeleeWeaponNF::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp || !MeshComp->GetOwner()) return;

	ACH4Character* Character = Cast<ACH4Character>(MeshComp->GetOwner());
	if (Character)
	{
		if (AActor* MeleeWeapon = Character->EquippableComponent->CurrentWeapon)
		{
			FOutputDeviceNull Ar;
			MeleeWeapon->CallFunctionByNameWithArguments(*FunctionName.ToString(), Ar, nullptr, true);
		}
	}
}