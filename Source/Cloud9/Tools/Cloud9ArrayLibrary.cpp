// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9ArrayLibrary.h"

#include "Extensions/TArray.h"

TArray<FString> UCloud9ArrayLibrary::SortArrayOfString(const TArray<FString>& Array)
{
	return Array | ETArray::Sorted{};
}

TArray<int32> UCloud9ArrayLibrary::SortArrayOfInt32(const TArray<int32>& Array) { return Array | ETArray::Sorted{}; }
