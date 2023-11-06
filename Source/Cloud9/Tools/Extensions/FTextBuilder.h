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

#pragma once

#include "Cloud9/Cloud9.h"

namespace EFTextBuilder
{
	struct AppendObject : TOperator<AppendObject>
	{
		AppendObject(const UObject* Object, const UStruct* Type)
			: Object(Object), Type(Type) { }

		FTextBuilder& operator()(FTextBuilder& Builder) const;

	private:
		const UObject* Object;
		const UStruct* Type;
	};

	struct AppendProperty : TOperator<AppendProperty>
	{
		AppendProperty(const UObject* Object, const FProperty* Property)
			: Object(Object), Property(Property) { }

		FTextBuilder& operator()(FTextBuilder& Builder) const;

	private:
		const UObject* Object;
		const FProperty* Property;
	};
	
	struct ToText : TOperator<ToText>
	{
		FText operator()(const FTextBuilder& Builder) const;
	};
}
