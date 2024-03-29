// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "QLAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API UQLAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UQLAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override; //지정된 변수 초기화
	virtual void NativeUpdateAnimation(float DeltaSeconds) override; //변수를 업데이트 
	
	//이 두 함수를 가져와서 GetOwningActor 함수를 사용해서 현재 어떤 상태를 가지고 있는지를 파악함. 
	//저장된 변수를 애님 그래프에서 사용, 애니메이션 자동 재생 
	
	//현재 이 애니메이션을 소유하고 있는 객체 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class ACharacter> Owner;
	//MovementComponent를 사용해서 해당 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character)
	TObjectPtr<class UCharacterMovementComponent> Movement;
	
	//속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	uint8 bisIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character)
	float MovingThreshold;
};
