// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeSurvivorController.h"
#include "EnhancedInputSubsystems.h"

AEscapeSurvivorController::AEscapeSurvivorController():
	bMappingContextAdded(false)
{
	SurvivorTeamID = FGenericTeamId(0);
}

FGenericTeamId AEscapeSurvivorController::GetGenericTeamId() const
{
	return SurvivorTeamID;
}


void AEscapeSurvivorController::ClientAddMappingContext_Implementation(UInputMappingContext* MappingContext, int32 Priority)
{
	if (!MappingContext)
	{
		return;
	}

	if (IsLocalController())
	{
		ULocalPlayer* LocalPlayer = GetLocalPlayer();

		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

			if (!bMappingContextAdded)
			{
				bMappingContextAdded = true;

				if (EnhancedInputLocalPlayerSubsystem)
				{
					EnhancedInputLocalPlayerSubsystem->AddMappingContext(MappingContext, Priority);

					//execute command "showdebug abilitysystem"
					LocalPlayer->ConsoleCommand(TEXT("showdebug abilitysystem"), true);
				}
			}
		}
	}
}

void AEscapeSurvivorController::ClientRemoveMappingContext_Implementation(UInputMappingContext* MappingContext)
{
	if (!MappingContext)
	{
		return;
	}

	if (IsLocalController())
	{
		ULocalPlayer* LocalPlayer = GetLocalPlayer();

		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

			if (bMappingContextAdded)
			{
				bMappingContextAdded = false;

				if (EnhancedInputLocalPlayerSubsystem)
				{
					EnhancedInputLocalPlayerSubsystem->RemoveMappingContext(MappingContext);

					LocalPlayer->ConsoleCommand(TEXT("showdebug abilitysystem"), true);
				}
			}
		}
	}
}

