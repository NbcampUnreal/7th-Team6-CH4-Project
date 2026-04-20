// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/SphereComponent.h"


// Sets default values
ABaseItem::ABaseItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(SceneComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	
	Tags.Add(TEXT("PickupItem"));
	
	bReplicates = true;
}



// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseItem::PlayWeaponShake()
{
    if (!CamShakeClass) return;

    //수류탄이 모든 플레이어의 화면을 흔듬
    if (WeaponPoseType == ECombatPose::BOOM)
    {
        UGameplayStatics::PlayWorldCameraShake(
            GetWorld(),
            CamShakeClass,
            GetActorLocation(), //현재 수류탄 위치
            InnerRadius,        //100% 강도 거리
            OuterRadius,        //0% 강도 거리
            1.0f,               //감쇄 지수
            false               //플레이어에게만 적용 여부
        );
    }
    //총기,근접은 가지고있는 플레이어만 흔들기
    else
    {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());

        //이 코드를 실행하는 클라이언트가 주인일 때만 실행
        if (OwnerPawn && OwnerPawn->IsLocallyControlled())
        {
            APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
            if (PC)
            {
                float FinalScale = 1.0f;

                //무기 자세별로 세부 강도 조정
                switch (WeaponPoseType)
                {
                case ECombatPose::Melee:      FinalScale = 0.8f;  break;
                case ECombatPose::Pistol:     FinalScale = 1.0f;  break;
                case ECombatPose::Shotgun:    FinalScale = 2.0f;  break;
                case ECombatPose::Rifle:      FinalScale = 1.3f;  break;
                case ECombatPose::MuchineGun: FinalScale = 0.7f;  break;
                default: break;
                }

                PC->ClientStartCameraShake(CamShakeClass, FinalScale);
            }
        }
    }
}