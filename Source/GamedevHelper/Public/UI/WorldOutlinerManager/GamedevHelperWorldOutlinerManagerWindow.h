﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GamedevHelperTypes.h"
#include "Widgets/SCompoundWidget.h"

class UGamedevHelperWorldOutlinerSettings;

class SWorldOutlinerManagerWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWorldOutlinerManagerWindow){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FReply OnOrganizeBtnClicked() const;
private:
	UGamedevHelperWorldOutlinerSettings* Settings = nullptr;
};