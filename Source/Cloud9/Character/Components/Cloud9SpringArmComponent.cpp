#include "Cloud9SpringArmComponent.h"

#include "Cloud9/Tools/Cloud9ToolsLibrary.h"


UCloud9SpringArmComponent::UCloud9SpringArmComponent()
{
	CameraLagVector = FVector::OneVector;
}

void UCloud9SpringArmComponent::UpdateDesiredArmLocation(
	bool bDoTrace,
	bool bDoLocationLag,
	bool bDoRotationLag,
	float DeltaTime)
{
	FRotator DesiredRot = GetTargetRotation();

	// Apply 'lag' to rotation if desired
	if (bDoRotationLag)
	{
		if (bUseCameraLagSubstepping && DeltaTime > CameraLagMaxTimeStep && CameraRotationLagSpeed > 0.f)
		{
			const FRotator ArmRotStep = (DesiredRot - PreviousDesiredRot).GetNormalized() * (1.f / DeltaTime);
			FRotator LerpTarget = PreviousDesiredRot;
			float RemainingTime = DeltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
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
	FVector ArmOrigin = GetComponentLocation() + TargetOffset;
	// We lag the target, not the actual camera position, so rotating the camera around does not have lag
	FVector DesiredLoc = ArmOrigin;
	if (bDoLocationLag)
	{
		const auto LocationLag = CameraLagSpeed * CameraLagVector;

		if (bUseCameraLagSubstepping
			&& DeltaTime > CameraLagMaxTimeStep
			&& LocationLag != FVector::ZeroVector)
		{
			const FVector ArmMovementStep = (DesiredLoc - PreviousDesiredLoc) * (1.f / DeltaTime);
			auto LerpTarget = PreviousDesiredLoc;

			float RemainingTime = DeltaTime;
			while (RemainingTime > KINDA_SMALL_NUMBER)
			{
				const float LerpAmount = FMath::Min(CameraLagMaxTimeStep, RemainingTime);
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

		if (const auto FromOrigin = DesiredLoc - ArmOrigin;
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
