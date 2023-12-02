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

#include "FTextBuilder.h"

auto LexToString(const FText& Text) { return *Text.ToString(); }

namespace EFTextBuilder
{
	template <typename TType, typename TResult = typename TType::TCppType>
	TResult UPropertyGetValue(const void* Object, const TType* Property)
	{
		let Ptr = Property->template ContainerPtrToValuePtr<void>(Object);
		return TType::GetPropertyValue(Ptr);
	}

	template <>
	bool UPropertyGetValue(const void* Object, const FBoolProperty* Property)
	{
		return Property->GetPropertyValue_InContainer(Object);
	}

	template <typename TValue>
	FString Format(const FProperty* Property, TValue Value)
	{
		let Name = Property->GetFName();
		let TypeName = Property->GetCPPType();
		return FString::Printf(TEXT("%s: %s = %s"), *Name.ToString(), *TypeName, *LexToString(Value));
	}

	template <typename TType>
	bool UPropertyAppendTo(FTextBuilder& Builder, const void* Object, const FProperty* Property)
	{
		if (let TypedProperty = CastField<TType>(Property))
		{
			let Value = UPropertyGetValue<TType>(Object, TypedProperty);
			let String = Format(Property, Value);
			Builder.AppendLine(String);
			return true;
		}
		return false;
	}

	template <>
	bool UPropertyAppendTo<FEnumProperty>(FTextBuilder& Builder, const void* Object, const FProperty* Property)
	{
		if (let TypedProperty = CastField<FEnumProperty>(Property))
		{
			let UnderlyingProperty = TypedProperty->GetUnderlyingProperty();
			let Ptr = Property->ContainerPtrToValuePtr<void>(Object);
			let Index = UnderlyingProperty->GetSignedIntPropertyValue(Ptr);
			let Enum = TypedProperty->GetEnum();
			let Value = Enum->GetNameByIndex(Index);
			let String = Format(Property, Value);
			Builder.AppendLine(String);
			return true;
		}
		return false;
	}

	template <>
	bool UPropertyAppendTo<FStructProperty>(FTextBuilder& Builder, const void* Object, const FProperty* Property)
	{
		if (let TypedProperty = CastField<FStructProperty>(Property))
		{
			let Ptr = TypedProperty->ContainerPtrToValuePtr<UObject>(Object);
			Builder | AppendObject(Ptr, TypedProperty->Struct);
			return true;
		}
		return false;
	}

	template <typename TType>
	bool UPropertyNotSupported(FTextBuilder& Builder, const void* Object, const FProperty* Property)
	{
		if (let TypedProperty = CastField<TType>(Property))
		{
			let String = Format(Property, TEXT("<UNSUPPORTED>"));
			Builder.AppendLine(String);
			return true;
		}
		return false;
	}

	FTextBuilder& AppendProperty::operator()(FTextBuilder&& Builder) const
	{
		UPropertyAppendTo<FInt16Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FIntProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FInt64Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FUInt16Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FUInt32Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FUInt64Property>(Builder, Object, Property)
			|| UPropertyAppendTo<FFloatProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FDoubleProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FBoolProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FObjectProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FWeakObjectProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FLazyObjectProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FSoftObjectProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FClassProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FSoftClassProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FInterfaceProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FNameProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FArrayProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FMapProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FSetProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FStructProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FDelegateProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FMulticastDelegateProperty>(Builder, Object, Property)
			|| UPropertyNotSupported<FMulticastInlineDelegateProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FEnumProperty>(Builder, Object, Property)
			|| UPropertyAppendTo<FTextProperty>(Builder, Object, Property);
		return Builder;
	}

	FTextBuilder& AppendObject::operator()(FTextBuilder&& Builder) const
	{
		let TypeClass = Type->GetClass();
		let Header = FString::Printf(TEXT("%s %s {"), *TypeClass->GetName(), *Type->GetName());

		Builder.AppendLine(Header);
		Builder.Indent();

		for (TFieldIterator<FProperty> It(Type); It; ++It)
		{
			Builder | AppendProperty(Object, *It);
		}

		Builder.Unindent();
		Builder.AppendLine(FString{"}"});

		return Builder;
	}

	FText ToText::operator()(FTextBuilder&& Builder) const { return Builder.ToText(); }
}
