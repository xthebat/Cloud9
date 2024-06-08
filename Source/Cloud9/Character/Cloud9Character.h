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

class UCloud9CharacterEffectTrait;
class UCloud9EffectsComponent;
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
	static inline const FName SpringArmComponentName = TEXT("CameraBoom");
	static inline const FName CameraComponentName = TEXT("TopDownCamera");
	static inline const FName DecalComponentName = TEXT("CursorToWorld");
	static inline const FName InventoryComponentName = TEXT("InventoryComponent");
	static inline const FName EffectsComponentName = TEXT("EffectsComponent");
	static inline const FName HealthComponentName = TEXT("HealthComponent");
	static inline const FName AnimationComponentName = TEXT("AnimationComponent");
	static inline const FName WidgetInteractionComponentName = TEXT("WidgetInteractionComponent");

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

	ACloud9Character(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	// Don't touch f...k EndPlay cus we got half-dead object in this callback
	// Override left just for the comment above
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void OnLevelChanged() const;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	ACloud9PlayerController* GetCloud9Controller() const;
	UCloud9CharacterMovement* GetCloud9CharacterMovement() const;

	bool CanSneak() const;

	void Sneak() const;

	void UnSneak() const;

	void SetViewDirection(const TOptional<FHitResult>& HitResult) const;

	UFUNCTION(BlueprintCallable)
	void SetDestroyAfter(float NewTime);

	UWidgetInteractionComponent* GetWidgetInteractionComponent() const;

	void SetCameraRotationYaw(float Angle) const;
	void AddCameraRotationYaw(float Angle) const;
	float GetCameraRotationRoll() const;
	void SetCameraRotationRoll(float Angle) const;

	float GetCameraZoomHeight() const;
	void SetCameraZoomHeight(float Value) const;

	bool GetNeedInitialize() const;

	UCloud9InventoryComponent* GetInventoryComponent() const;

	UCloud9HealthComponent* GetHealthComponent() const;

	UCloud9AnimationComponent* GetAnimationComponent() const;

	UCloud9CharacterEffectTrait* AddCharacterEffect(TSubclassOf<UCloud9CharacterEffectTrait> EffectClass);

	bool RemoveCharacterEffect(UCloud9CharacterEffectTrait* Effect);

	void RemoveAllCharacterEffects() const;

	void AddScore() const;

	void UseObject();

	void SetActorAimOffset(float Value);

	UFUNCTION(BlueprintCallable)
	float GetActorAimOffset() const;

	/** Event called when character score changed. */
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), Category=Events)
	FOnScoreChanged OnScoreChanged;

	// Set by character movement to specify that this Character is currently sneaking.
	// TODO: replicatedUsing=OnRep_IsSneaking
	UPROPERTY(BlueprintReadOnly, Category=Character)
	uint32 bIsSneaking : 1;

	bool IsHeadBone(FName Bone) const;

	bool IsUpperBodyBone(FName Bone) const;

	bool IsLowerBodyBone(FName Bone) const;

	bool IsLegsBone(FName Bone) const;

protected:
	UFUNCTION(BlueprintCallable)
	void OnCharacterDie(AActor* Actor);

	virtual float InternalTakePointDamage(
		float Damage,
		const FPointDamageEvent& PointDamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

private:
	/** "Temporary" hack field to control whether initialize character using game mode */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config, meta=(AllowPrivateAccess))
	bool bNeedInitialize;

	/** Time after which character will be destroyed after death */
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

	UPROPERTY(EditAnywhere, Category=Camera)
	float AimOffset;

	/** View direction will be calculated relative to this bone */
	UPROPERTY(EditDefaultsOnly, Category=Camera, meta=(AllowPrivateAccess))
	FName ViewHeadBoneName;

	/** Top-down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess))
	UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess))
	USpringArmComponent* CameraBoom;

	/** An inventory of the character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess))
	UCloud9InventoryComponent* InventoryComponent;

	/** A health and armor state of character. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess))
	UCloud9HealthComponent* HealthComponent;

	/** Helper to play animation montages for character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Utility, meta=(AllowPrivateAccess))
	UCloud9AnimationComponent* AnimationComponent;

	/** Helper to play animation montages for character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Utility, meta=(AllowPrivateAccess))
	UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Effects, meta=(AllowPrivateAccess))
	UCloud9EffectsComponent* EffectsComponent;
};
