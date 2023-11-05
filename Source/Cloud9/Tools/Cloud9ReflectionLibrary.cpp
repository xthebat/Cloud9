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

#include "Cloud9ReflectionLibrary.h"

#include "Cloud9/Cloud9.h"

auto LexToString(const FText& Text) { return *Text.ToString(); }

template <typename TType, typename TResult>
TResult UCloud9ReflectionLibrary::UPropertyGetValue(const UObject* Object, const TType* Property)
{
	let Ptr = Property->template ContainerPtrToValuePtr<void>(Object);
	return TType::GetPropertyValue(Ptr);
}

template <>
bool UCloud9ReflectionLibrary::UPropertyGetValue(const UObject* Object, const FBoolProperty* Property)
{
	return Property->GetPropertyValue_InContainer(Object);
}

template <typename TValue>
FString UCloud9ReflectionLibrary::FormatProperty(const FProperty* Property, TValue Value)
{
	let Name = Property->GetFName();
	let TypeName = Property->GetCPPType();
	return FString::Printf(TEXT("%s: %s = %s"), *Name.ToString(), *TypeName, *LexToString(Value));
}

template <typename TType>
TOptional<FString> UCloud9ReflectionLibrary::UPropertyConvert(const UObject* Object, const FProperty* Property)
{
	if (let TypedProperty = CastField<TType>(Property))
	{
		let Value = UPropertyGetValue<TType>(Object, TypedProperty);
		return FormatProperty(Property, Value);
	}
	return {};
}

template <>
TOptional<FString> UCloud9ReflectionLibrary::UPropertyConvert<FEnumProperty>(
	const UObject* Object,
	const FProperty* Property)
{
	if (let TypedProperty = CastField<FEnumProperty>(Property))
	{
		let UnderlyingProperty = TypedProperty->GetUnderlyingProperty();
		let Ptr = Property->ContainerPtrToValuePtr<void>(Object);
		let Index = UnderlyingProperty->GetSignedIntPropertyValue(Ptr);
		let Enum = TypedProperty->GetEnum();
		let Value = Enum->GetNameByIndex(Index);
		return FormatProperty(Property, Value);
	}
	return {};
}

template <typename TType>
bool UCloud9ReflectionLibrary::UPropertyAppendTo(
	FTextBuilder& Builder,
	const UObject* Object,
	const FProperty* Property)
{
	if (let String = UPropertyConvert<TType>(Object, Property))
	{
		Builder.AppendLine(*String);
		return true;
	}
	return false;
}

FString UCloud9ReflectionLibrary::UObjectToString(const UObject* Object)
{
	FTextBuilder Builder;
	let Class = Object->GetClass();

	Builder.AppendLine(FString::Printf(TEXT("class %s {"), *Class->GetName()));
	Builder.Indent();

	for (TFieldIterator<FProperty> PropertyIterator(Class); PropertyIterator; ++PropertyIterator)
	{
		let Property = *PropertyIterator;

		UPropertyAppendTo<FInt16Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FIntProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FInt64Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FUInt16Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FUInt32Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FUInt64Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FFloatProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FDoubleProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FBoolProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FObjectProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FWeakObjectProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FLazyObjectProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FSoftObjectProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FClassProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FSoftClassProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FInterfaceProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FNameProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FArrayProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FMapProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FSetProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FStructProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FDelegateProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FMulticastDelegateProperty>(Builder, Object, Property)
			// || UPropertyAppendTo<FMulticastInlineDelegateProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FEnumProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FTextProperty>(Builder, Object, Property);
	}

	Builder.Unindent();
	Builder.AppendLine(FString{"}"});

	let Text = Builder.ToText();

	return Text.ToString();
}
