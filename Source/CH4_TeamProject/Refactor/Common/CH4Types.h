#pragma once

#include "CoreMinimal.h"
#include "CH4Types.generated.h"

UENUM(BlueprintType)
enum class ECH4PlayerActionState : uint8
{
	None         UMETA(DisplayName = "None"),
	Hit          UMETA(DisplayName = "Hit"),
	Pickup       UMETA(DisplayName = "Pickup"),
	Down         UMETA(DisplayName = "Down"),
	Dead         UMETA(DisplayName = "Dead"),
	Revive       UMETA(DisplayName = "Revive"),
	PistolFire   UMETA(DisplayName = "PistolFire"),
	RifleFire    UMETA(DisplayName = "RifleFire"),
	PistolReload UMETA(DisplayName = "PistolReload"),
	RifleReload  UMETA(DisplayName = "RifleReload"),
	MeleeAttack  UMETA(DisplayName = "MeleeAttack"),
	DrinkHealItem UMETA(DisplayName = "DrinkHealItem"),
	ShotgunFire  UMETA(DisplayName = "ShotgunFire"),
	ShotgunReload UMETA(DisplayName = "ShotgunReload")
};

UENUM(BlueprintType)
enum class ECH4HitboxShape : uint8
{
	Sphere,
	Box,
	Capsule
};

// 플레이어 입력 액션 종류. 두 가지 용도로 동시에 사용:
//   1. ACH4PlayerCharacter::InputActions TMap 의 키 (IA 자산 조회).
//   2. ACH4PlayerCharacter::HandleInputAction 의 switch 분기 (Toggle/OneShot 계열만).
// Move/Look/Jump/Fire/Reload/EquipSlot1~4 는 (1) 용도로만 쓰이고 별도 메서드에 직접 바인딩.
UENUM(BlueprintType)
enum class ECH4PlayerInputAction : uint8
{
	// Axis / 단순 액션 (전용 메서드 직접 바인딩)
	Move       UMETA(DisplayName = "Move"),
	Look       UMETA(DisplayName = "Look"),
	Jump       UMETA(DisplayName = "Jump"),
	Fire       UMETA(DisplayName = "Fire"),
	Reload     UMETA(DisplayName = "Reload"),

	// Toggle (HandleInputAction 분기, Start/Completed 양쪽 바인딩)
	Sprint     UMETA(DisplayName = "Sprint"),
	Aim        UMETA(DisplayName = "Aim"),
	FreeLook   UMETA(DisplayName = "FreeLook"),

	// OneShot (HandleInputAction 분기, Started 한 곳)
	Interact   UMETA(DisplayName = "Interact"),
	Heal       UMETA(DisplayName = "Heal"),
	Throw      UMETA(DisplayName = "Throw"),

	// Equip 슬롯 (전용 메서드 직접 바인딩)
	EquipSlot1 UMETA(DisplayName = "EquipSlot1"),
	EquipSlot2 UMETA(DisplayName = "EquipSlot2"),
	EquipSlot3 UMETA(DisplayName = "EquipSlot3"),
	EquipSlot4 UMETA(DisplayName = "EquipSlot4")
};
