﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GamedevHelperAssetNamingManagerListRow.generated.h"

UCLASS(Transient)
class UGamedevHelperAssetNamingListItem : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FAssetData AssetData;
	UPROPERTY(VisibleAnywhere)
	FString OldName;
	UPROPERTY(VisibleAnywhere)
	FString NewName;
};

class SGamedevHelperAssetNamingListItem : public SMultiColumnTableRow<TWeakObjectPtr<UGamedevHelperAssetNamingListItem>>
{
public:
	SLATE_BEGIN_ARGS(SGamedevHelperAssetNamingListItem){}
		SLATE_ARGUMENT(TWeakObjectPtr<UGamedevHelperAssetNamingListItem>, RowItem)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
private:
	TWeakObjectPtr<UGamedevHelperAssetNamingListItem> RowItem;
};