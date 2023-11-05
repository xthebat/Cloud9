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
#include "Cloud9ReflectionLibrary.generated.h"

/**
 * Class to work with UE4 UPROPERTY/UCLASS reflection for debugging.
 */
UCLASS()
class CLOUD9_API UCloud9ReflectionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <typename TValue>
	static FString FormatProperty(const FProperty* Property, TValue Value);

	template <typename TType, typename TResult = typename TType::TCppType>
	static TResult UPropertyGetValue(const void* Object, const TType* Property);

	template <typename TType>
	static TOptional<FString> UPropertyConvert(const void* Object, const FProperty* Property);

	template <typename TType>
	static bool UPropertyAppendTo(FTextBuilder& Builder, const void* Object, const FProperty* Property);

	static bool UPropertyAppendTo(FTextBuilder& Builder, const void* Object, const FProperty* Property);

	UFUNCTION(BlueprintCallable)
	static FString UObjectToString(const UObject* Object, const UStruct* Type);
};
