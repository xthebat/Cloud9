#pragma once

#include "Cloud9/Cloud9.h"

// https://www.kevin-agwaze.com/logging-in-unreal-made-simple-with-helper-macros/

// Current Class Name + Function Name where this is called
#define TRACE_STR_CUR_CLASS_FUNC (FString(__FUNCTION__))

// Current Class where this is called
#define TRACE_STR_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )

// Current Function Name where this is called
#define TRACE_STR_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

// Current Line Number in the code where this is called
#define TRACE_STR_CUR_LINE  (FString::FromInt(__LINE__))

// Current Class and Line Number where this is called
#define TRACE_STR_CUR_CLASS_LINE (TRACE_STR_CUR_CLASS + ":" + TRACE_STR_CUR_LINE + " ")

// Current Class Name + Function Name and Line Number where this is called
#define TRACE_STR_CUR_CLASS_FUNC_LINE (TRACE_STR_CUR_CLASS_FUNC + ":" + TRACE_STR_CUR_LINE + " ")

// Current Function Signature where this is called
#define TRACE_STR_CUR_FUNCSIG (FString(__FUNCSIG__))

// Screen Message
#define screen(FormatString, ...) ( \
	GEngine->AddOnScreenDebugMessage( \
			-1, 10.0f, FColor::Yellow, \
			*(TRACE_STR_CUR_CLASS_FUNC_LINE + ": " + (FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ))) \
		) \
	)

#define log(Severity, FormatString, ...) \
	do { \
		UE_LOG(\
			LogCloud9, \
			Severity, \
			TEXT("%s: %s"), \
			*TRACE_STR_CUR_CLASS_FUNC_LINE, \
			*FString::Printf(TEXT(FormatString), ##__VA_ARGS__) \
		) \
	} \
	while (false)
