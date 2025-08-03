// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeSurvivorCharacter.h"
//Components
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//GAS related
#include "AbilitySystem/EscapeAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EscapeFunctionLibrary.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/EscapeInputComponent.h"
#include "DataAssets/StartUpData/DataAsset_SurvivorStartUpData.h"
#include "Components/Combat/SurvivorCombatComponent.h"
#include "Items/Weapons/EscapeSurvivorWeapon.h"
#include "Components/UI/SurvivorUIComponent.h"
#include "EscapeGameplayTags.h"
//Input
#include "EnhancedInputSubsystems.h"
//Kismet
#include "Kismet/KismetMathLibrary.h"
//Network
#include "Net/UnrealNetwork.h"
//Debug helper
#include "EscapeDebugHelper.h"

AEscapeSurvivorCharacter::AEscapeSurvivorCharacter():
	bCanFireWeapon(true),
	RespawnLocation(FVector(0.f, 0.f, 0.f))
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
	CameraBoom->bUsePawnControlRotation = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ThirdPersonCamera->bUsePawnControlRotation = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetMesh(), FirstPersonCameraSocketName);
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->bAutoActivate = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SurvivorCombatComponent = CreateDefaultSubobject<USurvivorCombatComponent>(TEXT("SurvivorCombatComponent"));
	SurvivorUIComponent = CreateDefaultSubobject<USurvivorUIComponent>(TEXT("SurvivorUIComponent"));
}

UPawnCombatComponent* AEscapeSurvivorCharacter::GetPawnCombatComponent() const
{
	return SurvivorCombatComponent;
}

UPawnUIComponent* AEscapeSurvivorCharacter::GetPawnUIComponent() const
{
	return SurvivorUIComponent;
}

USurvivorUIComponent* AEscapeSurvivorCharacter::GetSurvivorUIComponent() const
{
	return SurvivorUIComponent;
}

void AEscapeSurvivorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEscapeSurvivorCharacter::BeginPlay()
{
	Super::BeginPlay();

	RespawnLocation = GetActorLocation();
}

void AEscapeSurvivorCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEscapeSurvivorCharacter, bCanFireWeapon);
}

void AEscapeSurvivorCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(EscapeAbilitySystemComponent);
		}
	}
}

#pragma region Inputs

void AEscapeSurvivorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Missing InputConfigDataAsset"));

	ULocalPlayer* LocalPlayer =  GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(EnhancedInputLocalPlayerSubsystem);

	EnhancedInputLocalPlayerSubsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UEscapeInputComponent* EscapeInputComponent = CastChecked<UEscapeInputComponent>(PlayerInputComponent);
	EscapeInputComponent->BindNativeInputAction(InputConfigDataAsset, EscapeGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	EscapeInputComponent->BindNativeInputAction(InputConfigDataAsset, EscapeGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
	EscapeInputComponent->BindNativeInputAction(InputConfigDataAsset, EscapeGameplayTags::InputTag_Jump, ETriggerEvent::Triggered, this, &ThisClass::Input_Jump);
	EscapeInputComponent->BindNativeInputAction(InputConfigDataAsset, EscapeGameplayTags::InputTag_SwitchCameraMode, ETriggerEvent::Triggered, this, &ThisClass::InputTag_SwitchCameraMode);
	EscapeInputComponent->BindNativeInputAction(InputConfigDataAsset, EscapeGameplayTags::InputTag_Jump, ETriggerEvent::Completed, this, &ThisClass::Input_Jump_End);

	EscapeInputComponent->BindNativeInputAction(InputConfigDataAsset, EscapeGameplayTags::InputTag_FireWeapon, ETriggerEvent::Triggered, this, &ThisClass::ServerFireWeapon);
	EscapeInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputRelease);

}

void AEscapeSurvivorCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	FGameplayTagContainer TagsToCheck;
	TagsToCheck.AddTag(EscapeGameplayTags::Shared_Status_Dead);
	if (EscapeAbilitySystemComponent->HasAnyMatchingGameplayTags(TagsToCheck))
	{
		return;
	}

	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f, GetControlRotation().Yaw, 0.f);

	if (MovementVector.Y != 0.f)
	{
		const FVector ForwardDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection, MovementVector.Y);
	}
	if (MovementVector.X != 0.f)
	{
		const FVector RightDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AEscapeSurvivorCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}

	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AEscapeSurvivorCharacter::Input_Jump(const FInputActionValue& InputActionValue)
{
	Jump();
}

void AEscapeSurvivorCharacter::Input_Jump_End(const FInputActionValue& InputActionValue)
{
	StopJumping();
}

void AEscapeSurvivorCharacter::InputTag_SwitchCameraMode(const FInputActionValue& InputActionValue)
{
	if (ThirdPersonCamera->IsActive())
	{
		FirstPersonCamera->SetActive(true);
		ThirdPersonCamera->SetActive(false);

		GetMesh()->HideBoneByName(FName("head"), EPhysBodyOp::PBO_None);
	}
	else
	{
		FirstPersonCamera->SetActive(false);
		ThirdPersonCamera->SetActive(true);

		GetMesh()->UnHideBoneByName(FName("head"));
	}
}

void AEscapeSurvivorCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
	EscapeAbilitySystemComponent->OnAbilityInputPressed(InInputTag);

	//Debug::Print(TEXT(" Input Triggered "),FColor::Blue);
}

void AEscapeSurvivorCharacter::Input_AbilityInputRelease(FGameplayTag InInputTag)
{
	EscapeAbilitySystemComponent->OnAbilityInputRelease(InInputTag);
}

#pragma endregion

#pragma region Weapons

void AEscapeSurvivorCharacter::ServerFireWeapon_Implementation()
{
	//Require review, move weapoon related function to weapon itself

	if (!HasAuthority()) 
	{
		return;
	}

	if (!SurvivorCombatComponent)
	{
		return;
	}

	AEscapeSurvivorWeapon* CurrentEquipWeapon = SurvivorCombatComponent->GetSurvivorCarriedWeaponByTag(EscapeGameplayTags::Player_Weapon_Rifle);
	if (!CurrentEquipWeapon)
	{
		//Debug::Print(FString::Printf(TEXT("Missing CurrentEquipWeapon %s"), this->HasAuthority() ? TEXT("Server") : TEXT("Client")));

		return;
	}

	if (bCanFireWeapon)
	{
		CurrentEquipWeapon->FireWeapon();
		//Debug::Print(TEXT("Triggered Fire Weapon"));

		bCanFireWeapon = false;

		GetWorld()->GetTimerManager().SetTimer(FireCooldownHandle, this, &AEscapeSurvivorCharacter::ResetFiringState, 0.1f, false);
	}
}

void AEscapeSurvivorCharacter::ResetFiringState()
{
	bCanFireWeapon = true;
	//GetWorld()->GetTimerManager().ClearTimer(FireCooldownHandle);
}

#pragma endregion

#pragma region Animations

void AEscapeSurvivorCharacter::MulticastLinkAnimClassLayers_Implementation(USkeletalMeshComponent* TargetMesh, TSubclassOf<UAnimInstance> NewLayerClass)
{
	if (TargetMesh && NewLayerClass)
	{
		TargetMesh->LinkAnimClassLayers(NewLayerClass);
	}
}

void AEscapeSurvivorCharacter::MulticastUnlinkAnimClassLayers_Implementation(USkeletalMeshComponent* TargetMesh, TSubclassOf<UAnimInstance> NewLayerClass)
{
	if (TargetMesh && NewLayerClass)
	{
		TargetMesh->UnlinkAnimClassLayers(NewLayerClass);
	}
}

void AEscapeSurvivorCharacter::UpdateAimOffset(float DeltaSeconds)
{
	const FRotator ControlRotation = GetControlRotation();
	const FRotator ActorRotation = GetActorRotation();
	const FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);

	if (FMath::Abs(DeltaRot.Yaw) <= 160.f)
	{
		AimOffsetYaw = FMath::FInterpTo(AimOffsetYaw, DeltaRot.Yaw, DeltaSeconds, 6.f);
		AimOffsetYaw = FMath::Clamp(AimOffsetYaw, -160.f, 160.f);
	}
	else
	{
		AimOffsetYaw = FMath::FInterpTo(AimOffsetYaw, 0.f, DeltaSeconds, 6.f);
	}

	AimOffsetPitch = FMath::FInterpTo(AimOffsetPitch, DeltaRot.Pitch, DeltaSeconds, 6.f);
	AimOffsetPitch = FMath::Clamp(AimOffsetPitch, -90.f, 90.f);
}

void AEscapeSurvivorCharacter::MulticastAfterDeath_Implementation()
{
	//GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	GetMesh()->SetSimulatePhysics(true);
	GetWorld()->GetTimerManager().SetTimer(RespawnCountDownHandle, this, &AEscapeSurvivorCharacter::MulticasRespawn, 3.0f, false);

}

void AEscapeSurvivorCharacter::MulticasRespawn_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(RespawnCountDownHandle);

	FGameplayEventData EventData;
	EventData.Instigator = this;
	EventData.Target = this;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EscapeGameplayTags::Shared_Event_Respawn, EventData);

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	UEscapeFunctionLibrary::RemoveGameplayTagToActorIfFound(this, EscapeGameplayTags::Shared_Status_Dead);

	//
	//if (EscapeAbilitySystemComponent)
	//{
	//	EscapeAbilitySystemComponent->CancelAllAbilities();
	//	EscapeAbilitySystemComponent->RemoveActiveEffectsWithTags(FGameplayTagContainer::EmptyContainer);
	//	for (FGameplayTag tag : EscapeAbilitySystemComponent->GetOwnedGameplayTags())
	//	{
	//		EscapeAbilitySystemComponent->RemoveLooseGameplayTag(tag);
	//	}
	//}
}

#pragma endregion