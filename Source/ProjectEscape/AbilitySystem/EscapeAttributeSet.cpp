// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/EscapeAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "EscapeFunctionLibrary.h"
#include "EscapeGameplayTags.h"
#include "Interfaces/PawnUIInterface.h"
#include "Components/UI/PawnUIComponent.h"

UEscapeAttributeSet::UEscapeAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
	
	InitMaxWeaponAmmunitionAmount(1.f);
	InitCurrentWeaponAmmunitionAmount(1.f);
	InitAmmunitionToReload(1.f);
	InitSpareAmmunitionAmount(1.f);
	InitTotalAmmunitionConsumed(1.f);
}

void UEscapeAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}
	checkf(CachedPawnUIInterface.IsValid(), TEXT("%s is missing IPawnUIInterface"), *Data.Target.GetAvatarActor()->GetName());
	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();
	checkf(PawnUIComponent, TEXT("%s is failed to get PawnUIComponent"), *Data.Target.GetAvatarActor()->GetName());

#pragma region Character

	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float NewCurrentHealth = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());

		SetCurrentHealth(NewCurrentHealth);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float OldHealth = GetCurrentHealth();
		const float DamageDone = GetDamageTaken();

		const float NewCurrentHealth = FMath::Clamp(OldHealth - DamageDone, 0.f, GetMaxHealth());
		SetCurrentHealth(NewCurrentHealth);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
	}

#pragma endregion

#pragma region Weapon

	if (Data.EvaluatedData.Attribute == GetCurrentWeaponAmmunitionAmountAttribute())
	{
		PawnUIComponent->OnCurrentAmmunitionAmountChanged.Broadcast(GetCurrentWeaponAmmunitionAmount());
	}

	if (Data.EvaluatedData.Attribute == GetTotalAmmunitionConsumedAttribute())
	{
		const float NewCurrentAmount = FMath::Max(GetCurrentWeaponAmmunitionAmount() - GetTotalAmmunitionConsumed(), 0.f);
		SetCurrentWeaponAmmunitionAmount(NewCurrentAmount);

		PawnUIComponent->OnCurrentAmmunitionAmountChanged.Broadcast(GetCurrentWeaponAmmunitionAmount());
	}

	if (Data.EvaluatedData.Attribute == GetSpareAmmunitionAmountAttribute())
	{
		PawnUIComponent->OnSpareAmmunitionAmountChanged.Broadcast(GetSpareAmmunitionAmount());
	}

	if (Data.EvaluatedData.Attribute == GetAmmunitionToReloadAttribute())
	{
		if (GetCurrentWeaponAmmunitionAmount() < GetMaxWeaponAmmunitionAmount() && GetSpareAmmunitionAmount() > 0)
		{
			const float AmmoNeeded = FMath::Min(GetMaxWeaponAmmunitionAmount() - GetCurrentWeaponAmmunitionAmount(), GetSpareAmmunitionAmount());
			const float AmountToReload = FMath::Min(GetSpareAmmunitionAmount(), AmmoNeeded);

			const float NewCurrentAmount = FMath::Min(GetCurrentWeaponAmmunitionAmount() + AmountToReload, GetMaxWeaponAmmunitionAmount());
			SetCurrentWeaponAmmunitionAmount(NewCurrentAmount);

			const float NewSpareAmount = GetSpareAmmunitionAmount() - AmountToReload;
			SetSpareAmmunitionAmount(NewSpareAmount);

			PawnUIComponent->OnCurrentAmmunitionAmountChanged.Broadcast(GetCurrentWeaponAmmunitionAmount());
			PawnUIComponent->OnSpareAmmunitionAmountChanged.Broadcast(GetSpareAmmunitionAmount());
		}
	}

	
#pragma endregion
}

void UEscapeAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Character
	DOREPLIFETIME(UEscapeAttributeSet, CurrentHealth);
	DOREPLIFETIME(UEscapeAttributeSet, MaxHealth);
	DOREPLIFETIME(UEscapeAttributeSet, AttackPower);
	DOREPLIFETIME(UEscapeAttributeSet, DefensePower);
	DOREPLIFETIME(UEscapeAttributeSet, DamageTaken);
	//Weapon
	DOREPLIFETIME(UEscapeAttributeSet, MaxWeaponAmmunitionAmount);
	DOREPLIFETIME(UEscapeAttributeSet, CurrentWeaponAmmunitionAmount);
	DOREPLIFETIME(UEscapeAttributeSet, TotalAmmunitionConsumed);
	DOREPLIFETIME(UEscapeAttributeSet, AmmunitionToReload);
	DOREPLIFETIME(UEscapeAttributeSet, SpareAmmunitionAmount);
}

void UEscapeAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldValue)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(GetOwningActor());
	}

	if (CachedPawnUIInterface.IsValid())
	{
		if (UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent())
		{
			const float HealthRatio = GetCurrentHealth() / FMath::Max(1.f, GetMaxHealth());
			PawnUIComponent->OnCurrentHealthChanged.Broadcast(HealthRatio);
		}
	}
}

void UEscapeAttributeSet::OnRep_CurrentWeaponAmmunitionAmount(const FGameplayAttributeData& OldValue)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(GetOwningActor());
	}

	if (CachedPawnUIInterface.IsValid())
	{
		if (UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent())
		{
			const float NewCurrentAmmunitionAmount = GetCurrentWeaponAmmunitionAmount();
			PawnUIComponent->OnCurrentAmmunitionAmountChanged.Broadcast(NewCurrentAmmunitionAmount);
		}
	}
}

void UEscapeAttributeSet::OnRep_SpareAmmunitionAmount(const FGameplayAttributeData& OldValue)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(GetOwningActor());
	}

	if (CachedPawnUIInterface.IsValid())
	{
		if (UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent())
		{
			const float NewSpareAmmunitionAmount = GetSpareAmmunitionAmount();
			PawnUIComponent->OnSpareAmmunitionAmountChanged.Broadcast(NewSpareAmmunitionAmount);
		}
	}
}

