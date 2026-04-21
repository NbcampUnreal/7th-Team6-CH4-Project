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
			// ✅ 이 로그로 함수이름이랑 무기가 제대로 잡히는지 확인
			UE_LOG(LogTemp, Warning, TEXT("NF 호출됨 - 무기: %s, 함수: %s"), 
				*MeleeWeapon->GetName(), *FunctionName.ToString());

			FOutputDeviceNull Ar;
			MeleeWeapon->CallFunctionByNameWithArguments(*FunctionName.ToString(), Ar, nullptr, true);
		}
		else
		{
			// ✅ 무기가 없으면 여기 찍힘
			UE_LOG(LogTemp, Error, TEXT("NF - CurrentWeapon이 없음!"));
		}
	}
	else
	{
		// ✅ 캐릭터 캐스트 실패하면 여기 찍힘
		UE_LOG(LogTemp, Error, TEXT("NF - Character 캐스트 실패!"));
	}
}