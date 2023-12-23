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
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9ToolsLibrary.generated.h"

class ACloud9GameMode;

/**
 * Class provides different tools/helpers functions to minimize boilerplate code.
 */
UCLASS()
class CLOUD9_API UCloud9ToolsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Function sets collision complexity for StaticMesh.
	 * This required to fix CS:GO map collision (a lot of simple collisions are mess after import from Blender).
	 * StaticMesh->GetBodySetup()->CollisionTraceFlag can't be accessed directly from Python API.
	 */
	UFUNCTION(BlueprintCallable)
	static void SetCollisionComplexity(UStaticMesh* StaticMesh, uint8 CollisionTraceFlag);

	/**
	 * Function calculates a volume ratio between bounding box and simple collision mesh volume.
	 * Research variant to fix collisions of CS:GO map. 
	 */
	UFUNCTION(BlueprintCallable)
	static float CalculateCollisionVolumeScale(UStaticMesh* StaticMesh);

	/**
	 * Function try to get current game world or return nullptr if failed.
	 * NOTE: Only available when world has been created.
	 */
	UFUNCTION(BlueprintCallable)
	static UWorld* GetGameWorld();

	/**
	 * Function try to get current game mode or return nullptr if failed.
	 * NOTE: Only available when world has been created.
	 */
	UFUNCTION(BlueprintCallable)
	static ACloud9GameMode* GetGameMode();

	template <class Cls>
	static Cls* GetActorOfClass()
	{
		return Cast<Cls>(UGameplayStatics::GetActorOfClass(GetGameWorld(), Cls::StaticClass()));
	}

	template <class Cls>
	static TArray<Cls*> GetAllActorsOfClass()
	{
		TArray<AActor*> Actors = {};
		UGameplayStatics::GetAllActorsOfClass(GetGameWorld(), Cls::StaticClass(), Actors);
		return reinterpret_cast<TArray<Cls*>&>(Actors);
	}

	template <class T, class U>
	static T InverseLerp(const T& A, const T& B, const U& X) { return static_cast<T>((X - A) / (B - A)); }

	static FBox GetAccurateReferencePoseBounds(const USkeletalMesh* Mesh);

	UFUNCTION(BlueprintCallable)
	static void GetWidthHeightDepth(const FBox& Box, float& Width, float& Height, float& Depth);

	static FRotator RadiansToDegrees(const FRotator Rotator);

	static FVector VInterpTo(
		const FVector Current,
		const FVector Target,
		float DeltaTime,
		const FVector InterpSpeed);
};
