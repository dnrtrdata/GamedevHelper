﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GdhEditorTypes.h"
#include "Styling/SlateStyle.h"

class FGdhEditorStyle
{
public:
	static void Initialize();
	static void Shutdown();
	static void ReloadTextures();
	static const ISlateStyle& Get();
	static FName GetStyleSetName();
	static const FSlateBrush* GetIcon(const FString& Specifier);
	static FSlateColor GetColor(const FString& Specifier);
	static FString GetIconByStatus(const EGdhGenericStatus Status);
	static FLinearColor GetColorByStatus(const EGdhGenericStatus Status);
private:
	static TSharedRef<FSlateStyleSet> Create();
	static TSharedPtr<FSlateStyleSet> StyleInstance;
};
