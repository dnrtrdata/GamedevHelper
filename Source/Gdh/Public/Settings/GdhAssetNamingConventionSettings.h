﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GdhTypes.h"
#include "GdhAssetNamingConventionSettings.generated.h"

DECLARE_DELEGATE(FGdhAssetNamingConventionSettingsOnChangeDelegate);

UCLASS(Config = EditorPerProjectUserSettings, meta=(DisplayName="Asset Naming Convention Settings"))
class UGdhAssetNamingConventionSettings final : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UGdhAssetNamingConventionSettings();
	
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;
	virtual FString GetDesc() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	FGdhAssetNamingConventionSettingsOnChangeDelegate& OnChange();
private:
	FGdhAssetNamingConventionSettingsOnChangeDelegate OnChangeDelegate;;
};
