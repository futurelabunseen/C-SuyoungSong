// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/QLGA_AttackHitCheckUsingBomb.h"
#include "Engine/EngineTypes.h"
#include "Character/QLCharacterPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Item/QLBomb.h"
#include "GameData/QLDataManager.h"
#include "AttributeSet/QLAS_WeaponStat.h"
#include "EngineUtils.h"
#include "Physics/QLCollision.h"
#include "GameplayTag/GamplayTags.h"
#include "AbilitySystemComponent.h"
#include "Item/QLFirewall.h"
#include "QuadLand.h"
UQLGA_AttackHitCheckUsingBomb::UQLGA_AttackHitCheckUsingBomb()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void UQLGA_AttackHitCheckUsingBomb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(ActorInfo->AvatarActor.Get());

	TArray<AActor*> ChildActors;
	Character->GetAttachedActors(ChildActors,true);

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	for (const auto& AttachedChild : ChildActors)
	{
		Bomb = Cast<AQLBomb>(AttachedChild);

		if (Bomb!=nullptr)
		{
			Bomb->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			
			FVector OutVelocity = Character->GetCameraForward() * WeaponStat->GetAttackSpeed();
			Bomb->OnActorOverlapDelegate.BindUObject(this, &UQLGA_AttackHitCheckUsingBomb::OnCompletedCallback);
			Bomb->ThrowBomb(OutVelocity);
			break;
		}
	}
}

void UQLGA_AttackHitCheckUsingBomb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	Bomb = nullptr;
}

void UQLGA_AttackHitCheckUsingBomb::OnCompletedCallback()
{

	FCollisionQueryParams Params(SCENE_QUERY_STAT(DetectionPlayer), false);
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

	TArray<FHitResult> NearbyPlayers;
	//ItemDetectionSocket
	bool bResult = GetWorld()->SweepMultiByChannel(
		NearbyPlayers,
		Bomb->GetActorLocation(),
		Bomb->GetActorLocation(),
		FQuat::Identity,
		CCHANNEL_QLACTION,
		FCollisionShape::MakeSphere(WeaponStat->GetAttackDistance()),
		Params
	);
	
	if (bResult)
	{
		
		for (const auto& Player : NearbyPlayers)
		{
			AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(Player.GetActor());

			if (Character)
			{
				FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect);

				if (EffectSpecHandle.IsValid())
				{
					FGameplayEffectContextHandle EffectContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);;
					EffectContextHandle.AddOrigin(Bomb->GetActorLocation()); //Origin 발생 위치를 저장한다.
					EffectContextHandle.AddSourceObject(WeaponStat);

					UAbilitySystemComponent* TargetASC = Character->GetAbilitySystemComponent();
					EffectSpecHandle.Data->SetContext(EffectContextHandle);
					TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);

				}
			}
		}
	}

	if (HasAuthority(&CurrentActivationInfo))
	{
		MulticastRPCShowGameplayCue();
	}

	Bomb->SetLifeSpan(0.5f); //없어짐.
	
	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UQLGA_AttackHitCheckUsingBomb::MulticastRPCShowGameplayCue_Implementation()
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	if (SourceASC)
	{
		FGameplayCueParameters CueParam;
		CueParam.Location = Bomb->GetActorLocation();
		SpawnFire();
		//현재 ASC를 가져와서 ExecuteGameplayCue 실행 
		SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_EFFECT_FIREWALL, CueParam);
	}

}

void UQLGA_AttackHitCheckUsingBomb::SpawnFire()
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		FCollisionQueryParams Params(SCENE_QUERY_STAT(FireCollision), false);
		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
		const UQLAS_WeaponStat* WeaponStat = SourceASC->GetSet<UQLAS_WeaponStat>();

		TArray<FHitResult> NearbyActors;
		//ItemDetectionSocket
		bool bResult = GetWorld()->SweepMultiByChannel(
			NearbyActors,
			Bomb->GetActorLocation(),
			Bomb->GetActorLocation(),
			FQuat::Identity,
			CCHANNEL_QLFIRE,
			FCollisionShape::MakeSphere(WeaponStat->GetAttackDistance()),
			Params
		);

		if (bResult)
		{

			for (const auto& NearbyActor : NearbyActors)
			{
				//Actor라면.. 움직일 수 있음..
				AQLCharacterPlayer* Character = Cast<AQLCharacterPlayer>(NearbyActor.GetActor());
				FActorSpawnParameters SpawnParams;
				FVector Location = NearbyActor.GetActor()->GetActorLocation();

				if (Character)
				{
					AQLFirewall *FireActor = GetWorld()->SpawnActor<AQLFirewall>(Fire, Location, FRotator::ZeroRotator, SpawnParams);
					FireActor->AttachToActor(Character,FAttachmentTransformRules::KeepWorldTransform);
				}
				else
				{
					GetWorld()->SpawnActor<AQLFirewall>(Fire, Location, FRotator::ZeroRotator, SpawnParams);
				}
			}

		}
	}
}