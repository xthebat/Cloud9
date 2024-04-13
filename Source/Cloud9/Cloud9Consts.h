// Copyright (c) 2024 Alexei Gladkikh

#pragma once

/**
 * See cs_shareddefs.cpp in cstrike15_src
 */
namespace Cloud9Player
{
	constexpr float SpeedRun = 260.0f;
	constexpr float SpeedVip = 227.0f;
	constexpr float SpeedShield = 160.0f;
	constexpr float SpeedHas_Hostage = 200.0f;
	constexpr float SpeedStopped = 1.0f;
	constexpr float SpeedObserver = 900.0f;

	constexpr float SpeedDuckModifier = 0.34f;
	constexpr float SpeedWalkModifier = 0.52f;
	constexpr float SpeedClimbModifier = 0.34f;

	constexpr float HeavyArmorFlinchModifier = 0.5f;

	constexpr float DuckSpeedIdeal = 8.0f;
};


namespace Cloud9WeaponConsts
{
	constexpr int MaxBullets = 16;

	constexpr float MaxFallingPenalty = 2.0f; // Accuracy is never worse than 2x starting penalty
	constexpr float MovementCurve01Exponent = 0.25f;
	constexpr int PenetrationCount = 4;

	constexpr float WeaponRecoilDecayThreshold = 1.10;

	constexpr float RangeExponentCoefficient = 1e-6f;
	constexpr float ArmorCoefficient = 0.5f;
	constexpr float ArmorBonus = 0.5f;
};
