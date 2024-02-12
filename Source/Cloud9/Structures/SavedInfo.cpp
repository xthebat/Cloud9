// Copyright (c) 2024 Alexei Gladkikh

#include "SavedInfo.h"

void FSavedInfo::Reset()
{
	bIsLoadRequired = false;
	Players.Reset();
}
