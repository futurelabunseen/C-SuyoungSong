// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/QLPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemComponent.h"
#include "Player/QLPlayerState.h"
#include "Character/QLCharacterPlayer.h"
#include "UI/QLPlayerHUDWidget.h"
#include "UI/QLUserWidget.h"
#include "UI/QLInventory.h"
#include "QuadLand.h"
#include "AttributeSet/QLAS_PlayerStat.h"
#include "AttributeSet/QLAS_WeaponStat.h"

void AQLPlayerController::SetHiddenHUD(EHUDType UItype)
{

	if (IsLocalController())
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Hidden);
	}
}
void AQLPlayerController::SetVisibilityHUD(EHUDType UItype)
{
	if (IsLocalController())
	{
		HUDs[UItype]->SetVisibility(ESlateVisibility::Visible);
	}
}

void AQLPlayerController::CloseInventory()
{
	FInputModeGameOnly GameOnlyInputMode;
	SetHiddenHUD(EHUDType::Inventory);
	bShowMouseCursor = false;
	SetInputMode(GameOnlyInputMode);
}

void AQLPlayerController::CreateHUD()
{
	if (HUDClass.Num() == 0) return;

	if (!IsLocalPlayerController())
	{
		return;
	}
	
	AQLPlayerState* PS = GetPlayerState<AQLPlayerState>();
	if (!PS)
	{
		QL_LOG(QLNetLog, Warning, TEXT("PlayerState is not founded"));
		return;
	}

	for (const auto &HUD : HUDClass)
	{
		UUserWidget *Widget = CreateWidget<UUserWidget>(this, HUD.Value);
		Widget->AddToViewport();
		Widget->SetVisibility(ESlateVisibility::Visible);
		HUDs.Add(HUD.Key, Widget);
	}

	UQLUserWidget *Widget = Cast<UQLUserWidget>(HUDs[EHUDType::HUD]);

	Widget->ChangedAmmoCnt(PS->GetCurrentAmmoCnt());
	Widget->ChangedRemainingAmmo(PS->GetMaxAmmoCnt()); //임시값 삽입
	Widget->ChangedHPPercentage(PS->GetHealth(), PS->GetMaxHealth());
	Widget->ChangedStaminaPercentage(PS->GetStamina(), PS->GetMaxStamina());
	
	SetHiddenHUD(EHUDType::Inventory);
	//HUD 초기화
}

void AQLPlayerController::AddItemEntry(UObject* Item)
{

	if (IsLocalController())
	{
		UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);
		if (InventoryUI)
		{
			InventoryUI->AddItem(Item);
		}
	}
}

void AQLPlayerController::UpdateItemEntry(UObject* Item, int32 CurrentItemCnt)
{
	if (IsLocalController())
	{
		UQLInventory* InventoryUI = Cast<UQLInventory>(HUDs[EHUDType::Inventory]);
		if (InventoryUI)
		{
			InventoryUI->UpdateItemEntry(Item, CurrentItemCnt);
		}
	}
}

void AQLPlayerController::RemoveItemEntry(EItemType ItemIdx, int32 ItemCnt)
{
	//Player전달
	AQLCharacterPlayer* QLCharacter =Cast<AQLCharacterPlayer>(GetPawn());

	QL_LOG(QLNetLog, Warning, TEXT("this?QL"));
	if (QLCharacter)
	{
		QLCharacter->ServerRPCRemoveItem(ItemIdx,ItemCnt);
	}

}
