// TODO: 본 매크로들은 사용 클래스가 EditDefaultsOnly UPROPERTY 패턴으로 전환되면 일괄 제거 예정.
#pragma once

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "UObject/ConstructorHelpers.h"

// AssetPath  : 자산 폴더 경로 문자열 리터럴 (예: "/Game/Player/Input/Action"). 끝에 슬래시 없음.
// AssetName  : 자산 파일명 식별자 (따옴표 없음, 예: IA_Move). 파일명과 자산 내부 이름이 같다고 가정.
// MemberName : 대입 대상 UInputAction 멤버 변수.
//
// 매크로는 블록 스코프({})로 감싸 정적 Finder 변수가 호출별로 별도 저장소를 가지도록 한다.
#define LOAD_INPUT_ACTION(MemberName, AssetPath, AssetName)                                       \
	{                                                                                             \
		static ConstructorHelpers::FObjectFinder<UInputAction> Finder(                            \
			TEXT("/Script/EnhancedInput.InputAction'" AssetPath "/" #AssetName "." #AssetName "'")); \
		if (Finder.Succeeded()) MemberName = Finder.Object;                                       \
	}

// LOAD_INPUT_ACTION 의 TMap 변형. Map[Key] = LoadedAction 형태로 적재.
//
// MapMember : TMap<EnumType, TObjectPtr<UInputAction>> 타입 멤버.
// KeyValue  : 적재 시 사용할 enum 값 (예: ECH4PlayerInputAction::Move).
// AssetPath : 폴더 경로 문자열 리터럴.
// AssetName : 자산 파일명 식별자 (따옴표 없음).
#define LOAD_INPUT_ACTION_INTO_MAP(MapMember, KeyValue, AssetPath, AssetName)                     \
	{                                                                                             \
		static ConstructorHelpers::FObjectFinder<UInputAction> Finder(                            \
			TEXT("/Script/EnhancedInput.InputAction'" AssetPath "/" #AssetName "." #AssetName "'")); \
		if (Finder.Succeeded()) MapMember.Add(KeyValue, Finder.Object);                           \
	}

// EnhancedInputComponent::BindAction 호출 단축. 멤버 함수 안에서만 호출 (this 사용).
//
// InputComp  : EnhancedInputComponent 포인터 변수.
// OwnerClass : 멤버 함수가 정의된 클래스 이름.
// Action     : UInputAction* 멤버.
// Event      : ETriggerEvent enum 멤버 이름 (Triggered, Started, Completed 등).
// Method     : OwnerClass::Method 형태로 호출될 멤버 함수 이름.
//
// 사용 예 (SetupPlayerInputComponent 안):
//
//   BIND_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, MoveAction, Triggered, HandleMoveInput);
//   BIND_INPUT_ACTION(Enhanced, ACH4PlayerCharacter, JumpAction, Triggered, Jump);
#define BIND_INPUT_ACTION(InputComp, OwnerClass, Action, Event, Method)                           \
	(InputComp)->BindAction((Action), ETriggerEvent::Event, this, &OwnerClass::Method)

// BIND_INPUT_ACTION 의 가변 인자 변형. 핸들러에 추가 인자를 미리 박아 둘 때 사용.
// EnhancedInputComponent::BindAction 의 가변 템플릿 오버로드로 그대로 전달.
//
// 사용 예 (분기 키 + bool 활성 플래그를 사전 바인딩):
//
//   BIND_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, SprintAction, Started,
//       HandleInputAction, ECH4PlayerInputAction::Sprint, true);
//   BIND_INPUT_ACTION_PARAMS(Enhanced, ACH4PlayerCharacter, SprintAction, Completed,
//       HandleInputAction, ECH4PlayerInputAction::Sprint, false);
#define BIND_INPUT_ACTION_PARAMS(InputComp, OwnerClass, Action, Event, Method, ...)               \
	(InputComp)->BindAction((Action), ETriggerEvent::Event, this, &OwnerClass::Method, __VA_ARGS__)

// BIND_INPUT_ACTION 의 TMap 조회 변형. Map[Key] 가 nullptr 이면 바인딩 생략(silent).
//
// Map : TMap<EnumType, TObjectPtr<UInputAction>> 타입 멤버.
// Key : enum 값.
#define BIND_MAPPED_INPUT_ACTION(InputComp, OwnerClass, Map, Key, Event, Method)                  \
	do                                                                                            \
	{                                                                                             \
		UInputAction* IA__ = (Map).FindRef(Key);                                                  \
		if (IA__) (InputComp)->BindAction(IA__, ETriggerEvent::Event, this, &OwnerClass::Method); \
	} while (0)

// BIND_MAPPED_INPUT_ACTION 의 가변 인자 변형.
#define BIND_MAPPED_INPUT_ACTION_PARAMS(InputComp, OwnerClass, Map, Key, Event, Method, ...)      \
	do                                                                                            \
	{                                                                                             \
		UInputAction* IA__ = (Map).FindRef(Key);                                                  \
		if (IA__) (InputComp)->BindAction(IA__, ETriggerEvent::Event, this, &OwnerClass::Method, __VA_ARGS__); \
	} while (0)
