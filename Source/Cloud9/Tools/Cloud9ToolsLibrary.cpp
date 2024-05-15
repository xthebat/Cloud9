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

#include "Cloud9ToolsLibrary.h"

#include "Cloud9/Modes//Cloud9GameMode.h"

void UCloud9ToolsLibrary::SetCollisionComplexity(UStaticMesh* StaticMesh, uint8 CollisionTraceFlag)
{
	StaticMesh->GetBodySetup()->CollisionTraceFlag = static_cast<ECollisionTraceFlag>(CollisionTraceFlag);
}

bool UCloud9ToolsLibrary::SetCollisionProfile(const UStaticMesh* StaticMesh, FName CollisionProfile)
{
	var& DefaultInstance = StaticMesh->GetBodySetup()->DefaultInstance;

	if (CollisionProfile != DefaultInstance.GetCollisionProfileName())
	{
		DefaultInstance.SetCollisionProfileName(CollisionProfile);
		return true;
	}

	return false;
}

float UCloud9ToolsLibrary::CalculateCollisionVolumeScale(UStaticMesh* StaticMesh)
{
	let Scale = FVector{1.0f, 1.0f, 1.0f};
	let BoundingBoxVolume = StaticMesh->GetBoundingBox().GetVolume();
	let SimpleCollisionVolume = StaticMesh->GetBodySetup()->AggGeom.GetVolume(Scale);
	return SimpleCollisionVolume / BoundingBoxVolume;
}

UWorld* UCloud9ToolsLibrary::GetGameWorld() { return GEngine->GameViewport->GetWorld(); }

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

TArray<FString> UCloud9ToolsLibrary::GetObjectEditorProperties(UClass* Class)
{
	RETURN_IF_FAIL(IsValid(Class), {}, Error, "Input class is invalid");
	return TFieldIterator<FProperty>(Class)
		| ETContainer::FromIterator{}
		| ETContainer::Filter{[](let& It) { return It.HasAnyPropertyFlags(CPF_Edit); }}
		| ETContainer::Transform{[](let& It) { return It.GetName(); }}
		| ETContainer::ToArray{};
}
