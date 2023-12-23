﻿// Copyright (c) 2023 Alexei Gladkikh
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

#include "Cloud9ToolsLibrary.h"

#include "Cloud9/Game/Cloud9GameMode.h"

void UCloud9ToolsLibrary::SetCollisionComplexity(UStaticMesh* StaticMesh, uint8 CollisionTraceFlag)
{
	StaticMesh->GetBodySetup()->CollisionTraceFlag = static_cast<ECollisionTraceFlag>(CollisionTraceFlag);
}

float UCloud9ToolsLibrary::CalculateCollisionVolumeScale(UStaticMesh* StaticMesh)
{
	let Scale = FVector{1.0f, 1.0f, 1.0f};
	let BoundingBoxVolume = StaticMesh->GetBoundingBox().GetVolume();
	let SimpleCollisionVolume = StaticMesh->GetBodySetup()->AggGeom.GetVolume(Scale);
	return SimpleCollisionVolume / BoundingBoxVolume;
}

UWorld* UCloud9ToolsLibrary::GetGameWorld() { return GEngine->GameViewport->GetWorld(); }

ACloud9GameMode* UCloud9ToolsLibrary::GetGameMode()
{
	if (let MyWorld = GetGameWorld(); MyWorld != nullptr)
	{
		return Cast<ACloud9GameMode>(UGameplayStatics::GetGameMode(MyWorld));
	}

	log(Error, "Can't get game world now");

	return nullptr;
}

/**
 * https://dev.epicgames.com/community/snippets/13w/skeletal-mesh-get-accurate-reference-pose-bounds
 */
FBox UCloud9ToolsLibrary::GetAccurateReferencePoseBounds(const USkeletalMesh* Mesh)
{
	var Box = FBox(ForceInitToZero);

	if (not Mesh || not Mesh->GetPhysicsAsset()) { return {}; }

	for (let BodySetups : Mesh->GetPhysicsAsset()->SkeletalBodySetups)
	{
		let& RefSkeleton = Mesh->GetSkeleton()->GetReferenceSkeleton();
		var ComponentSpaceBoneTransform = FTransform::Identity;

		var BoneIndex = RefSkeleton.FindBoneIndex(BodySetups->BoneName);
		while (BoneIndex != INDEX_NONE)
		{
			let& BoneLocalTM = RefSkeleton.GetRefBonePose()[BoneIndex];
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
	let Size = Box.GetExtent();
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
	let ClampLerp = [](auto Current, auto Dist, auto Alpha, auto Target)
	{
		return Alpha <= 0.0f ? Target : Current + Dist * FMath::Clamp(Alpha, 0.0f, 1.0f);
	};

	// Distance to reach
	let Dist = Target - Current;

	// If distance is too small, just set the desired location
	if (Dist.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return Target;
	}

	let Alpha = DeltaTime * InterpSpeed;

	return {
		ClampLerp(Current.X, Dist.X, Alpha.X, Target.X),
		ClampLerp(Current.Y, Dist.Y, Alpha.Y, Target.Y),
		ClampLerp(Current.Z, Dist.Z, Alpha.Z, Target.Z),
	};
}
