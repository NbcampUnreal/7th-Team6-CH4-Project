#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CH4_TeamProject/DataBase/DataBase.h"
#include "CH4ItemBase.generated.h"

class UCH4ItemData;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class CH4_TEAMPROJECT_API ACH4ItemBase : public AActor
{
	GENERATED_BODY()

public:
	ACH4ItemBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TObjectPtr<UCH4ItemData> ItemData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
