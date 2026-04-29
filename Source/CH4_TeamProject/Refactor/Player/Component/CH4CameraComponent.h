#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CH4CameraComponent.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ACharacter;
class UCH4MovementComponent;

UCLASS(ClassGroup = (CH4), meta = (BlueprintSpawnableComponent))
class CH4_TEAMPROJECT_API UCH4CameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCH4CameraComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Character 생성자에서 만든 SpringArm/Camera 를 본 컴포넌트가 참조하도록 등록.
	// SpringArm 은 Character RootComponent 부착, Camera 는 SpringArm SocketName 부착 상태여야 한다.
	void RegisterCameraComponents(USpringArmComponent* InSpringArm, UCameraComponent* InCamera);

	void StartAim();
	void StopAim();
	void StartFreeLook();
	void StopFreeLook();

	void UpdateRotationMode() const;

	UFUNCTION(BlueprintCallable, Category = "Camera|Aim")
	bool IsAiming() const { return bIsAiming; }

	UFUNCTION(BlueprintCallable, Category = "Camera|FreeLook")
	bool IsFreeLook() const { return bIsFreeLook; }

protected:
	void UpdateAimCamera(const float DeltaTime) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|Aim", Replicated)
	bool bIsAiming = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera|FreeLook", Replicated)
	bool bIsFreeLook = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim", meta = (ClampMin = "0.0"))
	float DefaultArmLength = 470.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	FVector DefaultSocketOffset = FVector(0.0f, 25.0f, 20.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim", meta = (ClampMin = "1.0", ClampMax = "170.0"))
	float DefaultFOV = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim", meta = (ClampMin = "0.0"))
	float AimArmLength = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim")
	FVector AimSocketOffset = FVector(0.0f, 55.0f, 15.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim", meta = (ClampMin = "1.0", ClampMax = "170.0"))
	float AimFOV = 65.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Aim", meta = (ClampMin = "0.0"))
	float AimInterpSpeed = 10.0f;

private:
	ACharacter* GetOwnerCharacter() const;
	UCH4MovementComponent* FindMovementComponent() const;

	UPROPERTY(Transient)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(Transient)
	TObjectPtr<UCameraComponent> Camera;
};
