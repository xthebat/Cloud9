// Copyright (c) 2024 Alexei Gladkikh


#include "Cloud9KeyLibrary.h"

#include "Macro/Common.h"

FKey UCloud9KeyLibrary::TextToKey(const FText& Text) { return FKey(*Text.ToString()); }

FText UCloud9KeyLibrary::KeyToText(const FKey& Key)
{
	let Name = Key.GetFName();
	return FText::FromName(Name);
}

bool UCloud9KeyLibrary::IsKeyNameValid(const FText& Text)
{
	let Key = TextToKey(Text);
	return Key.IsValid();
}
