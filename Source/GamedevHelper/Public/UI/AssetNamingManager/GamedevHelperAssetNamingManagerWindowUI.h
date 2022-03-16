﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UGamedevHelperAssetNamingManagerSettings;
class UGamedevHelperAssetNamingListItem;

class SAssetNamingManagerWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetNamingManagerWindow){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
private:
	void ListUpdate();
	void ListSort();
	TSharedRef<ITableRow> OnGenerateRow(
		TWeakObjectPtr<UGamedevHelperAssetNamingListItem> InItem,
		const TSharedRef<STableViewBase>& OwnerTable
	) const;
	FReply OnRefreshBtnClick();
	void OnSort(EColumnSortPriority::Type SortPriority, const FName& Name, EColumnSortMode::Type SortMode);
	
	UGamedevHelperAssetNamingManagerSettings* Settings = nullptr;
	TSharedPtr<SListView<TWeakObjectPtr<UGamedevHelperAssetNamingListItem>>> ListView;
	TArray<TWeakObjectPtr<UGamedevHelperAssetNamingListItem>> AssetList;
	TEnumAsByte<EColumnSortMode::Type> CurrentSortMode = EColumnSortMode::Ascending;
	FName SortColumn;
};