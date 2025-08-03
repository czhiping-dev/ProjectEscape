// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EscapeBaseCharacter.h"
#include "GameplayTagContainer.h"
#include "EscapeSurvivorCharacter.generated.h"



class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
class USurvivorCombatComponent;
class USurvivorUIComponent;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class PROJECTESCAPE_API AEscapeSurvivorCharacter : public AEscapeBaseCharacter
{
	GENERATED_BODY()

	
public:

	AEscapeSurvivorCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;

	virtual UPawnUIComponent* GetPawnUIComponent() const override;

	virtual USurvivorUIComponent* GetSurvivorUIComponent() const override;

	virtual void UpdateAimOffset(float DeltaSeconds) override;

#pragma region Animations
	
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastLinkAnimClassLayers(USkeletalMeshComponent* TargetMesh,TSubclassOf<UAnimInstance> NewLayerClass);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastUnlinkAnimClassLayers(USkeletalMeshComponent* TargetMesh, TSubclassOf<UAnimInstance> NewLayerClass);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastAfterDeath();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticasRespawn();

#pragma endregion

#pragma region Weapons

	UFUNCTION(Server, Unreliable,BlueprintCallable)
	void ServerFireWeapon();

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite, Category = "Weapon")
	bool bCanFireWeapon;

	UFUNCTION()
	void ResetFiringState();

	FTimerHandle FireCooldownHandle;
	FTimerHandle RespawnCountDownHandle;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	FVector RespawnLocation;

#pragma endregion

protected:

	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:

#pragma region Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* ThirdPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FName FirstPersonCameraSocketName = "FirstPersonCameraSocket";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USurvivorCombatComponent* SurvivorCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	USurvivorUIComponent* SurvivorUIComponent;

#pragma endregion

#pragma region Inputs

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UDataAsset_InputConfig* InputConfigDataAsset;

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_Look(const FInputActionValue& InputActionValue);
	void Input_Jump(const FInputActionValue& InputActionValue);
	void Input_Jump_End(const FInputActionValue& InputActionValue);
	void InputTag_SwitchCameraMode(const FInputActionValue& InputActionValue);
	
	void Input_AbilityInputPressed(FGameplayTag InInputTag);
	void Input_AbilityInputRelease(FGameplayTag InInputTag);

#pragma endregion
public:

	FORCEINLINE USurvivorCombatComponent* GetSurvivorCombatComponent() const { return SurvivorCombatComponent; }
};

