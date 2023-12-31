// Copyright (c) 2023 Alexei Gladkikh
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Components/Cloud9Inventory.h"
#include "Components/Cloud9CharacterMovement.h"

#include "Cloud9Character.generated.h"


UCLASS(config=Game, Blueprintable)
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
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	ACloud9PlayerController* GetCloud9Controller() const;
	UCloud9CharacterMovement* GetCloud9CharacterMovement() const;

	bool CanSneak() const;

	void Sneak() const;

	void UnSneak() const;

	void SetViewDirection(const FHitResult& HitResult, bool bIsHitValid);

	UFUNCTION(BlueprintCallable)
	float GetViewVerticalRotation() const { return ViewVerticalRotation; }

	void SetCameraRotationYaw(float Angle) const;
	void AddCameraRotationYaw(float Angle) const;
	float GetCameraRotationRoll() const;
	void SetCameraRotationRoll(float Angle) const;

	void SetCursorIsHidden(bool Hidden) const;

	float GetCameraZoomHeight() const;
	void SetCameraZoomHeight(float Value) const;

	UCloud9Inventory* GetInventory() const;
	void UseObject();

	// Set by character movement to specify that this Character is currently sneaking.
	// TODO: replicatedUsing=OnRep_IsSneaking
	UPROPERTY(BlueprintReadOnly, Category=Character)
	uint32 bIsSneaking : 1;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ViewVerticalRotation;

private:
	// TODO: Move to GameInstance
	UPROPERTY(EditDefaultsOnly)
	UMaterial* CursorDecal;

	/** A decal that projects to the cursor location. */
	UPROPERTY(EditDefaultsOnly, Category=Camera, meta=(AllowPrivateAccess))
	FName CameraTargetBoneName;

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess))
	UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess))
	USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess))
	UDecalComponent* CursorToWorld;

	/** An inventory of the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess))
	UCloud9Inventory* Inventory;
};
