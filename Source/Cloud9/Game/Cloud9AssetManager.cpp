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

#include "Cloud9AssetManager.h"

#include "Cloud9/Tools/Macro/Logging.h"

UObject* UCloud9AssetManager::GetOrLoadAssetSync(
	FPrimaryAssetId PrimaryAssetId,
	const FString& Path,
	UClass* BaseClass,
	bool bHasBlueprintClasses,
	bool bIsEditorOnly)
{
	static var& Manager = Get();

	// Required to scan when using in OnConstruction method
	Manager.ScanPathForPrimaryAssets(
		PrimaryAssetId.PrimaryAssetType,
		Path,
		BaseClass,
		bHasBlueprintClasses,
		bIsEditorOnly,
		true
	);

	return GetOrLoadScannedAssetSync(PrimaryAssetId);
}

UObject* UCloud9AssetManager::GetOrLoadScannedAssetSync(FPrimaryAssetId PrimaryAssetId)
{
	static var& Manager = Get();

	let Future = Manager.LoadPrimaryAsset(PrimaryAssetId);
	StaticAssertOrReturn(Future, nullptr, Error, "Can't find asset: '%s'", *PrimaryAssetId.ToString());

	StaticAssertOrReturn(
		Future->WaitUntilComplete() == EAsyncPackageState::Complete, nullptr, Error,
		"Failed to loading asset: '%s'", *PrimaryAssetId.ToString());

	let Asset = Future->GetLoadedAsset();
	Asset->AddToRoot(); // Prevent GC destroy asset
	return Asset;
}
