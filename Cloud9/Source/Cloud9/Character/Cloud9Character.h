// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Cloud9Inventory.h"
#include "GameFramework/Character.h"
#include "Cloud9Character.generated.h"

class ACloud9PlayerController;
class UCloud9CharacterMovement;

UCLASS(Blueprintable)
class ACloud9Character : public ACharacter
{
	GENERATED_BODY()

public:
	static const FName SpringArmComponentName;
	static const FName CameraComponentName;
	static const FName DecalComponentName;
	static const FName InventoryComponentName;
	
	ACloud9Character(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;
	
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	bool CanSneak() const;

	void Sneak() const;

	void UnSneak() const;

	void SetViewDirection(const FHitResult& HitResult);

	void AddCameraRotationYaw(float Angle) const;
	float GetCameraRotationRoll() const;
	void SetCameraRotationRoll(float Angle) const;

	void SetCursorIsHidden(bool Hidden) const;

	float GetCameraZoomHeight() const;
	void SetCameraZoomHeight(float Value) const;
	
	UCloud9Inventory* GetInventory() const;

	// Set by character movement to specify that this Character is currently sneaking.
	// TODO: replicatedUsing=OnRep_IsSneaking
	UPROPERTY(BlueprintReadOnly, Category=Character)
	uint32 bIsSneaking:1;
	
private:
	const UCloud9CharacterMovement* GetMyCharacterMovement() const;
	const ACloud9PlayerController* GetMyPlayerController() const;

	UPROPERTY(EditDefaultsOnly)
	UMaterial* CursorDecal;

	/** A decal that projects to the cursor location. */
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	FName CameraTargetBoneName;
	
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* CursorToWorld;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	UCloud9Inventory* Inventory;
};
