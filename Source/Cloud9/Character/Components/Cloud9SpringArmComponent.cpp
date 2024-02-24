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

#include "Cloud9SpringArmComponent.h"
#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Game/Cloud9DeveloperSettings.h"
#include "Cloud9/Tools/Cloud9ToolsLibrary.h"

UCloud9SpringArmComponent::UCloud9SpringArmComponent()
{
	CameraLagVector = FVector::OneVector;
	CachedCameraVerticalSpeedLag = -1.0f;
}

// Origin code: Copyright Epic Games, Inc. All Rights Reserved.
void UCloud9SpringArmComponent::UpdateDesiredArmLocation(
	bool bDoTrace,
	bool bDoLocationLag,
	bool bDoRotationLag,
	float DeltaTime)
{
	let Settings = UCloud9DeveloperSettings::Get();
	if (let CameraVerticalSpeedLag = Settings->CameraVerticalSpeedLag; CameraVerticalSpeedLag > 0.0f)
	{
		CameraLagVector.Z = CameraVerticalSpeedLag / 10.0f;
	}
	else if (CachedCameraVerticalSpeedLag > 0.0f)
	{
		CameraLagVector.Z = CachedCameraVerticalSpeedLag;
	}

	var DesiredRot = GetTargetRotation();

	// Apply 'lag' to rotation if desired
	if (bDoRotationLag)
	{
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraRotationLagSpeed > 0.f)
		{
			let ArmRotStep = (DesiredRot - PreviousDesiredRot).GetNormalized() * (1.f / DeltaTime);
			var LerpTarget = PreviousDesiredRot;
			var RemainingTime = DeltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				let LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmRotStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredRot = FRotator(
					FMath::QInterpTo(
						FQuat(PreviousDesiredRot),
						FQuat(LerpTarget),
						LerpAmount,
						CameraRotationLagSpeed)
				);

				PreviousDesiredRot = DesiredRot;
			}
		}
		else
		{
			DesiredRot = FRotator(
				FMath::QInterpTo(
					FQuat(PreviousDesiredRot),
					FQuat(DesiredRot),
					DeltaTime,
					CameraRotationLagSpeed
				)
			);
		}
	}
	PreviousDesiredRot = DesiredRot;

	// Get the spring arm 'origin', the target we want to look at
	var ArmOrigin = GetComponentLocation() + TargetOffset;
	// We lag the target, not the actual camera position, so rotating the camera around does not have lag
	var DesiredLoc = ArmOrigin;
	if (bDoLocationLag)
	{
		let LocationLag = CameraLagSpeed * CameraLagVector;

		if (bUseCameraLagSubstepping
			&& DeltaTime > CameraLagMaxTimeStep
			&& LocationLag != FVector::ZeroVector)
		{
			let ArmMovementStep = (DesiredLoc - PreviousDesiredLoc) * (1.f / DeltaTime);
			var LerpTarget = PreviousDesiredLoc;

			var RemainingTime = DeltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				let LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
				LerpTarget += ArmMovementStep * LerpAmount;
				RemainingTime -= LerpAmount;

				DesiredLoc = UCloud9ToolsLibrary::VInterpTo(PreviousDesiredLoc, LerpTarget, LerpAmount, LocationLag);
				PreviousDesiredLoc = DesiredLoc;
			}
		}
		else
		{
			DesiredLoc = UCloud9ToolsLibrary::VInterpTo(PreviousDesiredLoc, DesiredLoc, DeltaTime, LocationLag);
		}

		if (let FromOrigin = DesiredLoc - ArmOrigin;
			CameraLagMaxDistance > 0.f && FromOrigin.SizeSquared() > FMath::Square(CameraLagMaxDistance))
		{
			DesiredLoc = ArmOrigin + FromOrigin.GetClampedToMaxSize(CameraLagMaxDistance);
		}
	}

	PreviousArmOrigin = ArmOrigin;
	PreviousDesiredLoc = DesiredLoc;

	// Now offset camera position back along our rotation
	DesiredLoc -= DesiredRot.Vector() * TargetArmLength;
	// Add socket offset in local space
	DesiredLoc += FRotationMatrix(DesiredRot).TransformVector(SocketOffset);

	// Do a sweep to ensure we are not penetrating the world
	FVector ResultLoc;
	if (bDoTrace && TargetArmLength != 0.0f)
	{
		bIsCameraFixed = true;
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetOwner());

		FHitResult Result;
		GetWorld()->SweepSingleByChannel(
			Result,
			ArmOrigin,
			DesiredLoc,
			FQuat::Identity,
			ProbeChannel,
			FCollisionShape::MakeSphere(ProbeSize),
			QueryParams);

		UnfixedCameraPosition = DesiredLoc;

		ResultLoc = BlendLocations(DesiredLoc, Result.Location, Result.bBlockingHit, DeltaTime);

		if (ResultLoc == DesiredLoc)
		{
			bIsCameraFixed = false;
		}
	}
	else
	{
		ResultLoc = DesiredLoc;
		bIsCameraFixed = false;
		UnfixedCameraPosition = ResultLoc;
	}

	// Form a transform for new world transform for camera
	FTransform WorldCamTM(DesiredRot, ResultLoc);
	// Convert to relative to component
	FTransform RelCamTM = WorldCamTM.GetRelativeTransform(GetComponentTransform());

	// Update socket location/rotation
	RelativeSocketLocation = RelCamTM.GetLocation();
	RelativeSocketRotation = RelCamTM.GetRotation();

	UpdateChildTransforms();
}

void UCloud9SpringArmComponent::OnRegister()
{
	Super::OnRegister();
	CachedCameraVerticalSpeedLag = CameraLagVector.Z;
}
