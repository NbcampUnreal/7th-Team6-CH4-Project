// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MeleeWeaponNF.generated.h"

//todo 애님 노티파이 맞춰서 콜리전 활성화하기
/**
 * 
 */
UCLASS()
class CH4_TEAMPROJECT_API UMeleeWeaponNF : public UAnimNotify
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Settings")
	FName FunctionName;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
