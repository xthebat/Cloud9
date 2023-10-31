#include "Cloud9ToolsLibrary.h"
#include "Cloud9/Game/Cloud9GameMode.h"

void UCloud9ToolsLibrary::SetCollisionComplexity(UStaticMesh* StaticMesh, uint8 CollisionTraceFlag)
{
	StaticMesh->GetBodySetup()->CollisionTraceFlag = static_cast<ECollisionTraceFlag>(CollisionTraceFlag);
}

float UCloud9ToolsLibrary::CalculateCollisionVolumeScale(UStaticMesh* StaticMesh)
{
	const auto Scale = FVector{1.0f, 1.0f, 1.0f};
	const auto BoundingBoxVolume = StaticMesh->GetBoundingBox().GetVolume();
	const auto SimpleCollisionVolume = StaticMesh->GetBodySetup()->AggGeom.GetVolume(Scale);
	return SimpleCollisionVolume / BoundingBoxVolume;
}

UWorld* UCloud9ToolsLibrary::GetWorld() { return GEngine->GameViewport->GetWorld(); }

ACloud9GameMode* UCloud9ToolsLibrary::GetGameMode()
{
	const auto MyWorld = GetWorld();
	return Cast<ACloud9GameMode>(UGameplayStatics::GetGameMode(MyWorld));
}

/**
 * https://dev.epicgames.com/community/snippets/13w/skeletal-mesh-get-accurate-reference-pose-bounds
 */
FBox UCloud9ToolsLibrary::GetAccurateReferencePoseBounds(const USkeletalMesh* Mesh)
{
	auto Box = FBox(ForceInitToZero);

	if (!Mesh || !Mesh->GetPhysicsAsset()) { return {}; }

	for (const auto BodySetups : Mesh->GetPhysicsAsset()->SkeletalBodySetups)
	{
		const FReferenceSkeleton& RefSkeleton = Mesh->GetSkeleton()->GetReferenceSkeleton();
		auto ComponentSpaceBoneTransform = FTransform::Identity;

		auto BoneIndex = RefSkeleton.FindBoneIndex(BodySetups->BoneName);
		while (BoneIndex != INDEX_NONE)
		{
			const FTransform& BoneLocalTM = RefSkeleton.GetRefBonePose()[BoneIndex];
			ComponentSpaceBoneTransform = ComponentSpaceBoneTransform * BoneLocalTM;

			BoneIndex = RefSkeleton.GetParentIndex(BoneIndex);
		}

		Box += BodySetups->AggGeom.CalcAABB(ComponentSpaceBoneTransform);
	}

	return Box;
}

/**
 * Use CS:GO models import Width/Height/Depth convention 
 */
void UCloud9ToolsLibrary::GetWidthHeightDepth(const FBox& Box, float& Width, float& Height, float& Depth)
{
	const auto Size = Box.GetExtent();
	Width = Size.Y;
	Height = Size.X;
	Depth = Size.Z;
}

FRotator UCloud9ToolsLibrary::RadiansToDegrees(const FRotator Rotator)
{
	return {
		FMath::RadiansToDegrees(Rotator.Pitch),
		FMath::RadiansToDegrees(Rotator.Yaw),
		FMath::RadiansToDegrees(Rotator.Roll)
	};
}

FVector UCloud9ToolsLibrary::VInterpTo(
	const FVector Current,
	const FVector Target,
	float DeltaTime,
	const FVector InterpSpeed)
{
	const auto ClampLerp = [](auto Current, auto Dist, auto Alpha, auto Target)
	{
		return Alpha <= 0.0f ? Target : Current + Dist * FMath::Clamp(Alpha, 0.0f, 1.0f);
	};

	// Distance to reach
	const auto Dist = Target - Current;

	// If distance is too small, just set the desired location
	if (Dist.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return Target;
	}

	const auto Alpha = DeltaTime * InterpSpeed;

	return {
		ClampLerp(Current.X, Dist.X, Alpha.X, Target.X),
		ClampLerp(Current.Y, Dist.Y, Alpha.Y, Target.Y),
		ClampLerp(Current.Z, Dist.Z, Alpha.Z, Target.Z),
	};
}
