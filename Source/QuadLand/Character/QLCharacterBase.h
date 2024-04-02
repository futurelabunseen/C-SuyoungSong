// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/AttackHitCheckInterface.h"
#include "QLCharacterBase.generated.h"

UCLASS()
class QUADLAND_API AQLCharacterBase : public ACharacter, public IAttackHitCheckInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQLCharacterBase();

	uint8 bHasGun : 1;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraSpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> Camera;

	/*AI와 같이 사용, InputAction*/
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrunchAction;

protected:
	
	virtual void DefaultAttack(); //NPC - Player 모두 총을 주웠을 때 사용할 예정
	virtual void AttackHitCheckUsingPunch() override;
	virtual void AttackHitCheckUsingGun() override;

	ECharacterAttackType CurrentAttackType;
	virtual FGameplayTag GetCurrentAttackTag() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Equipment, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Weapon;

	//Attack
protected:
};
