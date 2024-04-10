// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/QLReceivedDamageInterface.h"
#include "Interface/AttackHitCheckInterface.h"
#include "QLCharacterBase.generated.h"

UCLASS()
class QUADLAND_API AQLCharacterBase : public ACharacter, public IAttackHitCheckInterface, public IQLReceivedDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AQLCharacterBase();

	uint8 bHasGun : 1;

	/*AI�� ���� ���, InputAction*/
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrunchAction;

protected:
	ECharacterAttackType CurrentAttackType;
	virtual FGameplayTag GetCurrentAttackTag() const override;
	//Attack
protected:

	//TMap���� ��������. Gun ���� �� Montage�� Gun ���� �� Montage �� ����� ���� - enum���� �з� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimMontage)
	TMap<ECharacterAttackType, TObjectPtr<class UAnimMontage>> AttackAnimMontage;

};