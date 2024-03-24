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
#include "Cloud9/Tools/Extensions/TContainer.h"

BEGIN_DEFINE_SPEC(
	FRangeSpec,
	"Range.Unit",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

END_DEFINE_SPEC(FRangeSpec)

void FRangeSpec::Define()
{
	// Capture by value because otherwise Container maybe destroyed in test

	Describe("TSet", [this]
	{
		let Container = TSet{1, 2, 3, 4, 5};

		It("Random", [=]
		{
			let Value = Container | ETContainer::Random();
			TestTrue("Value.IsSet()", Value.IsSet());
			TestTrue("Collection.Contains(Value)", Container.Contains(*Value));
		});
	});

	Describe("TFieldIterator", [this]
	{
		It("FromIterator", [=]
		{
			let Expected = TArray<FString>{"BoolField", "IntField", "FloatField"};

			let Actual = TFieldIterator<FProperty>(UWhenOrNoneTestClassA::StaticClass())
				| ETContainer::FromIterator{}
				| ETContainer::Transform{[](let& It) { return It.GetName(); }}
				| ETContainer::ToArray{};

			TestEqual("Actual == {BoolField, IntField, FloatField}", Actual, Expected);
		});
	});

	Describe("TArray", [this]
	{
		let Container = TArray{1, 2, 3, 4, 5};

		It("Random", [=]
		{
			let Value = Container | ETContainer::Random();
			TestTrue("Value.IsSet()", Value.IsSet());
			TestTrue("Collection.Contains(Value)", Container.Contains(*Value));
		});

		It("ForEach", [=]
		{
			var Result = 0;
			Container | ETContainer::ForEach{[&](let It) { Result += It; }};
			TestEqual("Result == 15", Result, 15);
		});

		It("ToArray", [=]
		{
			{
				let Result = Container | ETContainer::ToArray{.ToDrop = 3, .ToTake = 1};
				TestEqual("Result == {4}", Result, {4});
			}

			{
				let Result = Container | ETContainer::ToArray{.ToTake = 0};
				TestEqual("Take Result == {}", Result, {});
			}

			{
				let Result = Container | ETContainer::ToArray{.ToDrop = 100500};
				TestEqual("Drop Result == {}", Result, {});
			}
		});

		It("Filter", [=]
		{
			let Result = Container
				| ETContainer::Filter{[&](let It) { return It > 2; }}
				| ETContainer::ToArray{};
			TestEqual("Result == {3, 4, 5}", Result, {3, 4, 5});
		});

		It("Transform", [=]
		{
			{
				let Result = Container
					| ETContainer::Transform{[&](let It) { return It * 2; }}
					| ETContainer::ToArray{};
				TestEqual("Result == {2, 4, 6, 8, 10}", Result, {2, 4, 6, 8, 10});
			}

			{
				let Result = Container
					| ETContainer::Transform{[&](let It) { return It; }}
					| ETContainer::ToArray{};
				TestEqual("Result == {1, 2, 3, 4, 5}", Result, {1, 2, 3, 4, 5});
			}
		});

		It("Associate", [=]
		{
			let Result = Container
				| ETContainer::Associate
				{
					[&](let It) { return It; },
					[&](let It) { return static_cast<float>(It) / 2.0f; }
				};

			let Expected = TMap<int, float>{
				{1, 0.5f},
				{2, 1.0f},
				{3, 1.5f},
				{4, 2.0f},
				{5, 2.5f},
			};

			for (let It : Result)
			{
				let Key = It.Get<0>();
				let Value = It.Get<1>();
				TestTrue("Contains Result / 2.0f", Expected.Contains(Key));
				TestEqual("Result / 2.0f", Value, Expected[Key]);
			}
		});
	});
}
