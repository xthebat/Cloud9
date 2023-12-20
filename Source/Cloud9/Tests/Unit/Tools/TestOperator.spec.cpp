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


#include "Cloud9/Cloud9.h"
#include "Cloud9/Tools/Extensions/TArray.h"
#include "Cloud9/Tools/Macro/Operator.h"

BEGIN_DEFINE_SPEC(
	FOperatorSpec,
	"Operator.Unit",
	EAutomationTestFlags::ProductFilter | EAutomationTestFlags::ApplicationContextMask)

END_DEFINE_SPEC(FOperatorSpec)

void FOperatorSpec::Define()
{
	Describe("TIsOperator", [this]
	{
		It("FilterIsOperator", [&]
		{
			constexpr let Filter = ETArray::Filter{[] { return true; }};
			TestTrue("TIsOperator<decltype(Filter)>::Value", TIsOperator<decltype(Filter)>::Value);
		});

		It("TArrayIsNotOperator", [&]
		{
			let Array = TArray{1, 2, 3};
			TestTrue("TIsOperator<decltype(Array)>::Value)", TIsOperator<decltype(Array)>::Value);
		});
	});
}
