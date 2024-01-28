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
#include "Engine/AssetManager.h"

#include "Cloud9/Tools/Macro/Common.h"
#include "Cloud9/Tools/Macro/Logging.h"

#include "Cloud9AssetManager.generated.h"

UCLASS()
class CLOUD9_API UCloud9AssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	/**
	 * Scans a path and reads asset data for all primary assets of a specific type.
     * If done in the editor it will load the data off disk, in cooked games it will
     * load out of the asset registry cache.
     * 
     * Then loads primary asset and adds loaded asset to root (AddToRoot) to prevent it from destroy by GC.
     * 
	 * @param PrimaryAssetId Asset Id to load
	 * @param Path Path to scan of assets
	 * @param BaseClass 
	 * @param bHasBlueprintClasses 
	 * @param bIsEditorOnly 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable)
	static UObject* GetOrLoadAssetSync(
		FPrimaryAssetId PrimaryAssetId,
		const FString& Path,
		UClass* BaseClass,
		bool bHasBlueprintClasses = false,
		bool bIsEditorOnly = false);

	/**
	 * Template functions wrapper for GetOrLoadAssetSync
	 *  
	 * @tparam ClassType Class type of asset to look for
	 * @param bHasBlueprintClasses If true, the assets are blueprints that subclass BaseClass.
	 *                             If false they are base UObject assets
	 * @param bIsEditorOnly If true, assets will only be scanned in editor builds,
	 *                      and will not be stored into the asset registry
	 * @return Object of loaded asset
	 */
	template <typename ClassType>
	static ClassType* GetOrLoadAssetSync(bool bHasBlueprintClasses = false, bool bIsEditorOnly = false)
	{
		let Asset = GetOrLoadAssetSync(
			ClassType::PrimaryAssetId,
			ClassType::Path,
			ClassType::StaticClass(),
			bHasBlueprintClasses,
			bIsEditorOnly
		);

		return Cast<ClassType>(Asset);
	}

	/**
	 * Functions loads primary asset only if it was scanned (e.g. won't work for PIE start OnConstruction method).
	 * Also function adds loaded asset to root (AddToRoot) to prevent it from destroy by GC.
	 * 
	 * @param PrimaryAssetId Asset Id to load
	 * @return Object of loaded asset
	 */
	UFUNCTION(BlueprintCallable)
	static UObject* GetOrLoadScannedAssetSync(FPrimaryAssetId PrimaryAssetId);

	/**
	 * Template functions wrapper for GetOrLoadScannedAssetSync
	 * 
	 * @param PrimaryAssetId Asset Id to load
	 * @return Object of loaded asset
	 */
	template <typename ClassType>
	static ClassType* GetOrLoadScannedAssetSync(FPrimaryAssetId PrimaryAssetId = ClassType::PrimaryAssetId)
	{
		return Cast<ClassType>(GetOrLoadScannedAssetSync(PrimaryAssetId));
	}
};
