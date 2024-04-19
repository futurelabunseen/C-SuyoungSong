// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystemInterface.h"
#include "QLPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class QUADLAND_API AQLPlayerController : public APlayerController , public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AQLPlayerController();

	void SetVisibleFarming();
	void SetInvisibleFarming();
	const class UUserWidget* GetCrossHairUIWidget() const { return CrossHairUI; }
	class UQLUserWidget* GetPlayerUIWidget() const { return StatUI; }
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;
protected:

	// Server only
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
	TSubclassOf<class UUserWidget> CrossHairHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
	TSubclassOf<class UQLUserWidget> PlayerStatHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
	TObjectPtr<class UUserWidget> CrossHairUI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widget)
	TObjectPtr<class UQLUserWidget> StatUI;

	void CreateHUD();

	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
