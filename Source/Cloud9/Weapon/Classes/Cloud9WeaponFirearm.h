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
#include "Cloud9/Tools/Structures.h"

#include "Cloud9/Weapon/Classes/Cloud9WeaponBase.h"
#include "Cloud9/Weapon/Enums/FirearmNames.h"

#include "Cloud9WeaponFirearm.generated.h"

class AStaticMeshActor;
class UCooldownActionComponent;
struct FWeaponActionMontages;
struct FFirearmWeaponInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoInReserveChanged, int, Count);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmmoInMagazineChanged, int, Count);

// Maybe just remove verbose logging?
UENUM()
enum class EFirearmFireStatus : uint8
{
	Success = 0,
	NoCursorHit = 1,

	PlayAnimation = NoCursorHit,

	OutOfAmmo = 2,
	Error = 0x80
};

USTRUCT()
struct FCursorHitScanInfo
{
	GENERATED_BODY()

	FVector StartLocation;
	FVector EndLocation;
	float Alpha = 0.0f;

	UPROPERTY()
	TArray<AActor*> ActorsToIgnore;

	FVector Direction() const { return EndLocation - StartLocation; }

	FVector ExtendedEndLocation() const { return FMath::Lerp(StartLocation, EndLocation, Alpha); }

	static TErrorValue<EFirearmFireStatus, FCursorHitScanInfo> Create(
		const ACloud9WeaponFirearm* Firearm,
		const FFirearmWeaponInfo* WeaponInfo,
		const FFirearmCommonData& FirearmCommonData);
};

UCLASS()
class CLOUD9_API ACloud9WeaponFirearm : public ACloud9WeaponBase
{
	GENERATED_BODY()

	friend class ACloud9WeaponBase;
	friend class UCloud9AnimNotifyPlaySound;
	friend class UCloud9AnimNotifyMagazine;

public:
	static const FName TracerProbabilityParameterName;
	static const FName TracerDirectionParameterName;

public:
	virtual FWeaponId GetWeaponId() const override;

	template <typename WeaponIdType>
	WeaponIdType GetWeaponId() const;

	const FFirearmWeaponInfo* GetWeaponInfo() const;

	int GetCurrentAmmo() const;
	int GetAmmoInReserve() const;
	bool AddAmmoInReserve(int Count);

protected:
	virtual bool OnInitialize(const FWeaponConfig& WeaponConfig) override;

	virtual void OnWeaponAddedToInventory() override;
	virtual void OnWeaponRemovedFromInventory() override;

	virtual void Tick(float DeltaSeconds) override;

	EFirearmFireStatus PrimaryAttack(const FFirearmWeaponInfo* WeaponInfo, const FFirearmCommonData& FirearmCommonData);

	EFirearmFireStatus GunFire(
		const FFirearmWeaponInfo* WeaponInfo,
		const FFirearmCommonData& FirearmCommonData,
		const FCursorHitScanInfo& HitScanInfo);

	bool UpdateReloadAmmo(bool IsShotgun);

	float GetInaccuracy() const;
	TArray<FVector> CalcShootInaccuracy(const FCursorHitScanInfo& HitScanInfo, float Seed = 0) const;

	bool UpdateMagazineAttachment(bool IsReload);
	void DropMagazine() const;
	void EjectCase() const;

protected: // properties
	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), Category=Events)
	FOnAmmoInReserveChanged OnAmmoInReserveChanged;

	UPROPERTY(BlueprintAssignable, meta=(AllowPrivateAccess), Category=Events)
	FOnAmmoInMagazineChanged OnAmmoInMagazineChanged;

	/**
	 * Weapon Identifier
	 */
	UPROPERTY(Category=Weapon, EditDefaultsOnly, meta=(AllowPrivateAccess))
	EFirearm WeaponId;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int CurrentAmmo;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int AmmoInReserve;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int MaxMagazineSize;

	UPROPERTY(Category=Weapon, BlueprintReadOnly, meta=(AllowPrivateAccess))
	int MaxAmmoInReserve;

	UPROPERTY()
	float AccuracyPenalty;

	UPROPERTY()
	int RecoilIndex;
};

template <>
inline EFirearm ACloud9WeaponFirearm::GetWeaponId<EFirearm>() const { return WeaponId; }
