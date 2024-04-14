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
#include "Components/Cloud9CharacterMovement.h"

#include "Cloud9/Tools/Macro/Common.h"

#include "Cloud9Character.generated.h"

class UWidgetInteractionComponent;
class ACloud9PlayerController;
class UCloud9InventoryComponent;
class UCloud9AnimationComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int, Count);

class UCloud9HealthComponent;

UCLASS(config=Game, Blueprintable)
class ACloud9Character : public ACharacter
{
	GENERATED_BODY()

public:
	static const FName SpringArmComponentName;
	static const FName CameraComponentName;
	static const FName DecalComponentName;
	static const FName InventoryComponentName;
	static const FName HealthComponentName;
	static const FName AnimationComponentName;
	static const FName WidgetInteractionComponentName;

	static constexpr let HeavyArmorFlinchModifier = 0.5f;

	static constexpr let DuckSpeedIdeal = 8.0f;

	static constexpr let DefaultDestroyAfterTime = 10.0f;
	static constexpr let CapsuleHeight = 144.0f;
	static constexpr let CapsuleRadius = 32.0f;
	static constexpr let MeshRotationYaw = -90.0f;
	static constexpr let CameraBoomYaw = -60.0f;
	static constexpr let JumpZVelocity = 320.0f;
	static constexpr let CrosshairRotationPitch = -90.0f;
	static constexpr let CanStepUpOn = ECB_Yes;

public:
	ACloud9Character(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
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

	void SetViewDirection(const TOptional<FHitResult>& HitResult);

	UFUNCTION(BlueprintCallable)
	void SetDestroyAfter(float NewTime);

	UFUNCTION(BlueprintCallable)
	float GetViewVerticalRotation() const { return ViewVerticalRotation; }

	UWidgetInteractionComponent* GetWidgetInteractionComponent() const;

	void SetCameraRotationYaw(float Angle) const;
	void AddCameraRotationYaw(float Angle) const;
	float GetCameraRotationRoll() const;
	void SetCameraRotationRoll(float Angle) const;

	void SetCursorIsHidden(bool Hidden) const;

	float GetCameraZoomHeight() const;
	void SetCameraZoomHeight(float Value) const;

	UCloud9InventoryComponent* GetInventoryComponent() const;

	UCloud9HealthComponent* GetHealthComponent() const;

	UCloud9AnimationComponent* GetAnimationComponent() const;

	void AddScore();

	void UseObject();

public:
	/** Event called when character score changed. */
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), Category=Events)
	FOnScoreChanged OnScoreChanged;

	// Set by character movement to specify that this Character is currently sneaking.
	// TODO: replicatedUsing=OnRep_IsSneaking
	UPROPERTY(BlueprintReadOnly, Category=Character)
	uint32 bIsSneaking : 1;

protected:
	UFUNCTION(BlueprintCallable)
	void OnCharacterDie(AActor* Actor);

	virtual float InternalTakePointDamage(
		float Damage,
		const FPointDamageEvent& PointDamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

private:
	/**
	 * Time after which character will be destroyed after death
	 */
	UPROPERTY(EditAnywhere, Category=Config)
	float DestroyAfterTime;

	UPROPERTY(EditDefaultsOnly, Category=Config, meta=(AllowPrivateAccess))
	TSet<FName> HeadBoneNames;

	UPROPERTY(EditDefaultsOnly, Category=Config, meta=(AllowPrivateAccess))
	TSet<FName> UpperBodyBoneNames;

	UPROPERTY(EditDefaultsOnly, Category=Config, meta=(AllowPrivateAccess))
	TSet<FName> LowerBodyBoneNames;

	UPROPERTY(EditDefaultsOnly, Category=Config, meta=(AllowPrivateAccess))
	TSet<FName> LegsBoneNames;

	// TODO: Move to decals asset
	UPROPERTY(EditDefaultsOnly, Category=Crosshair, meta=(AllowPrivateAccess))
	UMaterial* CursorDecal;

	UPROPERTY(EditDefaultsOnly, Category=Crosshair, meta=(AllowPrivateAccess))
	FVector CursorSize;

	UPROPERTY(EditDefaultsOnly, Category=Camera)
	float ViewVerticalRotation;

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
	UCloud9InventoryComponent* InventoryComponent;

	/** A state of the character health and armor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	UCloud9HealthComponent* HealthComponent;

	/** Helper to play animation montages for character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Utility, meta=(AllowPrivateAccess))
	UCloud9AnimationComponent* AnimationComponent;

	/** Helper to play animation montages for character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Utility, meta=(AllowPrivateAccess))
	UWidgetInteractionComponent* WidgetInteractionComponent;

	/** Current number of frags made by character */
	UPROPERTY(BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	int Score;
};
