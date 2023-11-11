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


#include "Cloud9/Tests/Unit/Fake/UWhenOrNoneTestClassA.h"
#include "Cloud9/Tests/Unit/Fake/UWhenOrNoneTestClassB.h"
#include "Cloud9/Tools/Extensions/WhenOrNone.h"

BEGIN_DEFINE_SPEC(
	FWhenOrNoneSpec,
	"WhenOrNone.Unit",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

END_DEFINE_SPEC(FWhenOrNoneSpec)

namespace FWhenOrNoneClasses
{
	class FBase
	{
	public:
		virtual ~FBase() = default;
		virtual int Hello() = 0;
	};

	class FDerived1 final : public FBase
	{
	public:
		virtual int Hello() override { return 1; }
	};

	class FDerived2 final : public FBase
	{
	public:
		virtual int Hello() override { return 2; }
	};

	class FDerived3 final : public FBase
	{
	public:
		virtual int Hello() override { return 3; }
	};

	class FDerived4 final : public FBase
	{
	public:
		virtual int Hello() override { return 4; }
	};
}

void FWhenOrNoneSpec::Define()
{
	Describe("WhenOrNone", [this]
	{
		let Object = UWhenOrNoneTestClassA::StaticClass()->GetDefaultObject<UWhenOrNoneTestClassA>();

		It("dynamic_cast", [this, Object]
		{
			// WARNING: Won't work without RTTI information (by default disabled in UE)

			// let Array = TArray<FWhenOrNoneClasses::FBase*>{
			// 	new FWhenOrNoneClasses::FDerived1,
			// 	new FWhenOrNoneClasses::FDerived2,
			// 	new FWhenOrNoneClasses::FDerived3,
			// 	new FWhenOrNoneClasses::FDerived4,
			// };
			//
			// TArray<int> Types;
			//
			// for (let Iterator : Array)
			// {
			// 	let TypeName = Iterator | WhenOrNone{
			// 		[this](FWhenOrNoneClasses::FDerived1* Arg) { return Arg->Hello(); },
			// 		[this](FWhenOrNoneClasses::FDerived2* Arg) { return Arg->Hello(); },
			// 		[this](FWhenOrNoneClasses::FDerived3* Arg) { return Arg->Hello(); },
			// 		[this](FWhenOrNoneClasses::FDerived4* Arg) { return Arg->Hello(); }
			// 	};
			// 	TestTrue("TypeName.IsSet()", TypeName.IsSet());
			// 	Types.Add(*TypeName);
			// }
			//
			// TestEqual("FDerived1", Types[0], 1);
			// TestEqual("FDerived2", Types[1], 2);
			// TestEqual("FDerived3", Types[2], 3);
			// TestEqual("FDerived4", Types[3], 4);
		});

		It("UObject", [this, Object]
		{
			UObject* AsUObject = Object;

			let TypeName = AsUObject | WhenOrNone{
				[this](UWhenOrNoneTestClassA* Arg) { return "UWhenOrNoneTestClassA"; },
				[this](UWhenOrNoneTestClassB* Arg) { return "UWhenOrNoneTestClassB"; }
			};
			TestTrue("TypeName.IsSet()", TypeName.IsSet());
			TestEqual("UWhenOrNoneTestClassA", *TypeName, "UWhenOrNoneTestClassA");
		});

		It("FProperty", [this, Object]
		{
			var Types = TArray<FString>();

			for (TFieldIterator<FProperty> Iterator(Object->GetClass()); Iterator; ++Iterator)
			{
				let TypeName = *Iterator | WhenOrNone{
					[this](FBoolProperty* Arg) { return "FBoolProperty"; },
					[this](FIntProperty* Arg) { return "FIntProperty"; },
					[this](FFloatProperty* Arg) { return "FFloatProperty"; }
				};
				TestTrue("TypeName.IsSet()", TypeName.IsSet());
				Types.Add(*TypeName);
			}

			TestEqual("FBoolProperty", Types[0], "FBoolProperty");
			TestEqual("FIntProperty", Types[1], "FIntProperty");
			TestEqual("FFloatProperty", Types[2], "FFloatProperty");
		});
	});
}
