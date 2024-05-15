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
#define CLOUD9_SCREEN_MESSAGE(FormatString, ...) ( \
	GEngine->AddOnScreenDebugMessage( \
			-1, 10.0f, FColor::Yellow, \
			*(TRACE_STR_CUR_CLASS_FUNC_LINE + ": " + (FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ))) \
		) \
	)

#define CLOUD9_LOG(Severity, FormatString, ...) do { \
	UE_LOG(\
		LogCloud9, \
		Severity, \
		TEXT("%s: %s"), \
		*TRACE_STR_CUR_CLASS_FUNC_LINE, \
		*FString::Printf(TEXT(FormatString), ##__VA_ARGS__) \
	) \
} while (false)

#define FUNCTION_FATAL(Message, ...) do { \
	CLOUD9_LOG(Fatal, Message, ##__VA_ARGS__); \
} while (false)

#define FUNCTION_ERROR(Message, ...) do { \
	CLOUD9_LOG(Error, Message, ##__VA_ARGS__); \
} while (false)

#define FUNCTION_WARNING(Message, ...) do { \
	CLOUD9_LOG(Warning, Message, ##__VA_ARGS__); \
} while (false)

#define FUNCTION_DISPLAY(Message, ...) do { \
	CLOUD9_LOG(Display, Message, ##__VA_ARGS__); \
} while (false)

#define FUNCTION_VERBOSE(Message, ...) do { \
	CLOUD9_LOG(Verbose, Message, ##__VA_ARGS__); \
} while (false)

#define OBJECT_FATAL(Message, ...) do { \
	CLOUD9_LOG(Fatal, "[%s]: %s", *GetName(), *FString::Printf(TEXT(Message), ##__VA_ARGS__)); \
} while (false)

#define OBJECT_ERROR(Message, ...) do { \
	CLOUD9_LOG(Error, "[%s]: %s", *GetName(), *FString::Printf(TEXT(Message), ##__VA_ARGS__)); \
} while (false)

#define OBJECT_WARN(Message, ...) do { \
	CLOUD9_LOG(Warning, "[%s]: %s", *GetName(), *FString::Printf(TEXT(Message), ##__VA_ARGS__)); \
} while (false)

#define OBJECT_DISPLAY(Message, ...) do { \
	CLOUD9_LOG(Display, "[%s]: %s", *GetName(), *FString::Printf(TEXT(Message), ##__VA_ARGS__)); \
} while (false)

#define OBJECT_VERBOSE(Message, ...) do { \
	CLOUD9_LOG(Verbose, "[%s]: %s", *GetName(), *FString::Printf(TEXT(Message), ##__VA_ARGS__)); \
} while (false)
