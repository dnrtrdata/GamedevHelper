﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#include "GamedevHelperSubsystem.h"
#include "GamedevHelper.h"
#include "UI/GamedevHelperEditorStyle.h"
// Engine Headers
#include "ToolMenus.h"
#include "ContentBrowserModule.h"
#include "EditorAssetLibrary.h"
#include "IContentBrowserSingleton.h"
#include "Misc/FeedbackContext.h"
#include "Framework/Notifications/NotificationManager.h"

#define LOCTEXT_NAMESPACE "FGamedevHelper"

void UGamedevHelperSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	RegisterContextMenuActions();
}

void UGamedevHelperSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UGamedevHelperSubsystem::ConfigureStaticMeshForVertexAnim(UStaticMesh* StaticMesh)
{
	if (!StaticMesh) return false;
	if (!StaticMesh->IsValidLowLevel()) return false;

	DisableCollision(StaticMesh);

	StaticMesh->Modify();

	for (int32 LODIndex = 0; LODIndex < StaticMesh->GetNumLODs(); ++LODIndex)
	{
		FStaticMeshSourceModel& SourceModel = StaticMesh->GetSourceModel(LODIndex);
		SourceModel.BuildSettings.bRemoveDegenerates = false;
		SourceModel.BuildSettings.bUseFullPrecisionUVs = true;
		SourceModel.BuildSettings.bGenerateLightmapUVs = false;
		SourceModel.BuildSettings.DistanceFieldResolutionScale = 0.0f;
	}

	StaticMesh->PostEditChange();

	if (StaticMesh->MarkPackageDirty())
	{
		return UEditorAssetLibrary::SaveLoadedAsset(StaticMesh, true);
	}

	return false;
}

void UGamedevHelperSubsystem::ConfigureStaticMeshesForVertexAnim(TArray<UStaticMesh*> StaticMeshes, const bool bShowNotification)
{
	if (!GWarn) return;
	if (StaticMeshes.Num() == 0) return;

	bool bResult = true;

	GWarn->BeginSlowTask(FText::FromString(GamedevHelperStandardText::ConfiguringStaticMeshes), true);

	for (const auto& StaticMesh : StaticMeshes)
	{
		if (!ConfigureStaticMeshForVertexAnim(StaticMesh))
		{
			bResult = false;
		}
	}

	GWarn->EndSlowTask();

	if (!bShowNotification)
	{
		return;
	}

	if (bResult)
	{
		ShowModal(GamedevHelperStandardText::ConfiguringStaticMeshesSuccess, EGamedevHelperModalStatus::Success, 3.0f);
	}
	else
	{
		ShowModalWithOutputLog(GamedevHelperStandardText::ConfiguringStaticMeshesFail);
	}
}

bool UGamedevHelperSubsystem::ConfigureTextureForVertexAnim(UTexture2D* Texture, const EGamedevHelperVertexAnimTexture TextureType)
{
	if (!Texture) return false;

	Texture->SRGB = false;
	Texture->CompressionSettings = TextureType == EGamedevHelperVertexAnimTexture::UV ? TC_HDR : TC_VectorDisplacementmap;
	Texture->Filter = TF_Nearest;
	Texture->NeverStream = true;
	Texture->MipGenSettings = TMGS_NoMipmaps;
	Texture->LODGroup = TEXTUREGROUP_UI;
	Texture->PostEditChange();

	if (Texture->MarkPackageDirty())
	{
		return UEditorAssetLibrary::SaveLoadedAsset(Texture, true);
	}

	return false;
}

void UGamedevHelperSubsystem::ConfigureTexturesForVertexAnim(TArray<UTexture2D*> Textures, const EGamedevHelperVertexAnimTexture TextureType, const bool bShowNotification)
{
	if (!GWarn) return;
	if (Textures.Num() == 0) return;

	bool bResult = true;

	GWarn->BeginSlowTask(FText::FromString(GamedevHelperStandardText::ConfiguringTextures), true);

	for (const auto& Texture : Textures)
	{
		if (!ConfigureTextureForVertexAnim(Texture, TextureType))
		{
			bResult = false;
		}
	}

	GWarn->EndSlowTask();

	if (!bShowNotification)
	{
		return;
	}

	if (bResult)
	{
		ShowModal(GamedevHelperStandardText::ConfiguringTexturesSuccess, EGamedevHelperModalStatus::Success, 3.0f);
	}
	else
	{
		ShowModalWithOutputLog(GamedevHelperStandardText::ConfiguringTexturesFail);
	}
}

void UGamedevHelperSubsystem::GetSelectedAssets(TArray<FAssetData>& SelectedAssets)
{
	const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
}

bool UGamedevHelperSubsystem::DisableCollision(UStaticMesh* StaticMesh)
{
	if (!StaticMesh) return false;
	if (!StaticMesh->IsValidLowLevel()) return false;

	UBodySetup* BodySetup = StaticMesh->GetBodySetup();
	if (!BodySetup) return false;

	StaticMesh->Modify();

	FBodyInstance BodyInstance = BodySetup->DefaultInstance;
	BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyInstance.SetCollisionProfileName(FName{"NoCollision"});

	BodySetup->DefaultInstance = BodyInstance;
	StaticMesh->SetBodySetup(BodySetup);
	StaticMesh->PostEditChange();

	if (StaticMesh->MarkPackageDirty())
	{
		return UEditorAssetLibrary::SaveLoadedAsset(StaticMesh, true);
	}

	return false;
}

void UGamedevHelperSubsystem::DisableCollisions(TArray<UStaticMesh*> StaticMeshes, const bool bShowNotification)
{
	if (!GWarn) return;
	if (StaticMeshes.Num() == 0) return;

	bool bResult = true;

	GWarn->BeginSlowTask(FText::FromString(GamedevHelperStandardText::DisablingStaticMeshesCollision), true);

	for (const auto& StaticMesh : StaticMeshes)
	{
		if (!DisableCollision(StaticMesh))
		{
			bResult = false;
		}
	}

	GWarn->EndSlowTask();

	if (!bShowNotification)
	{
		return;
	}

	if (bResult)
	{
		ShowModal(GamedevHelperStandardText::DisablingStaticMeshesCollisionSuccess, EGamedevHelperModalStatus::Success, 3.0f);
	}
	else
	{
		ShowModalWithOutputLog(GamedevHelperStandardText::DisablingStaticMeshesCollisionFail);
	}
}

void UGamedevHelperSubsystem::ShowModal(const FString& Msg, const EGamedevHelperModalStatus ModalStatus, const float Duration)
{
	const FText NotificationText = FText::FromString(Msg);
	FNotificationInfo Info{NotificationText};
	Info.ExpireDuration = Duration;
	const auto Notification = FSlateNotificationManager::Get().AddNotification(Info);
	if (Notification.IsValid())
	{
		Notification.Get()->SetCompletionState(GetCompletionStateFromModalStatus(ModalStatus));
	}
}

void UGamedevHelperSubsystem::ShowModalWithOutputLog(const FString& Msg, const float Duration)
{
	const FText NotificationText = FText::FromString(Msg);
	FNotificationInfo Info{NotificationText};
	Info.ExpireDuration = Duration;
	Info.Hyperlink = FSimpleDelegate::CreateStatic([]() { FGlobalTabmanager::Get()->TryInvokeTab(FName("OutputLog")); });
	Info.HyperlinkText = NSLOCTEXT("GamedevHelperShowOutputLog", "GamedevHelperShowOutputLog", "Show OutputLog...");
	FSlateNotificationManager::Get().AddNotification(Info);
}

void UGamedevHelperSubsystem::ShowModalWithHyperLink(const FString& Msg, const FString& Link, const float Duration)
{
	const FText NotificationText = FText::FromString(Msg);
	FNotificationInfo Info{NotificationText};
	Info.ExpireDuration = Duration;
	Info.Hyperlink = FSimpleDelegate::CreateStatic([](FString InDirectory) { FPlatformProcess::ExploreFolder(*InDirectory); }, Link);
	Info.HyperlinkText = NSLOCTEXT("GamedevHelperOpenFolder", "GamedevHelperOpenFolder", "Open Folder...");
	FSlateNotificationManager::Get().AddNotification(Info);
}

void UGamedevHelperSubsystem::RegisterContextMenuActions() const
{
	// static mesh actions
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.StaticMesh");
		FToolMenuSection& Section = Menu->FindOrAddSection("GamedevHelperActions");

		Section.AddSubMenu(
			"GamedevHelperSubMenu",
			LOCTEXT("GamedevHelperSubMenu", "GamedevHelper Actions"),
			LOCTEXT("GamedevHelperSubMenu_ToolTip", "GamedevHelper plugin asset helper actions"),
			FNewMenuDelegate::CreateLambda([&](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.BeginSection("Section_VAT", FText::FromString("Vertex Animation Tools"));
				MenuBuilder.AddMenuEntry(
					LOCTEXT("VAT_StaticMesh", "Configure"),
					LOCTEXT("VAT_StaticMesh_ToolTip", "Configure selected static meshes for vertex animation"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([]()
						{
							TArray<UStaticMesh*> StaticMeshes;
							GetSelectedAssets<UStaticMesh>(StaticMeshes);

							ConfigureStaticMeshesForVertexAnim(StaticMeshes);
						})
					)
				);
				MenuBuilder.EndSection();
				MenuBuilder.BeginSection("Section_Utility", FText::FromString("Utility"));
				MenuBuilder.AddMenuEntry(
					LOCTEXT("Utility_DisableCollision", "Disable Collision"),
					LOCTEXT("Utility_DisableCollision_ToolTip", "Disable collision on selected static meshes"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([]()
						{
							TArray<UStaticMesh*> StaticMeshes;
							GetSelectedAssets<UStaticMesh>(StaticMeshes);

							DisableCollisions(StaticMeshes);
						})
					)
				);
				MenuBuilder.EndSection();
			}),
			false,
			FSlateIcon(FGamedevHelperEditorStyle::GetStyleSetName(), "GamedevHelper.Icon16")
		);
	}

	// texture asset actions
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.Texture");
		FToolMenuSection& Section = Menu->FindOrAddSection("GamedevHelperActions");

		Section.AddSubMenu(
			"GamedevHelperSubMenu",
			LOCTEXT("GamedevHelperSubMenu", "GamedevHelper Actions"),
			LOCTEXT("GamedevHelperSubMenu_ToolTip", "GamedevHelper plugin asset helper actions"),
			FNewMenuDelegate::CreateLambda([&](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.BeginSection("Section_VAT", FText::FromString("Vertex Animation Tools"));
				MenuBuilder.AddMenuEntry(
					LOCTEXT("VAT_TextureNormal", "Configure as Normal"),
					LOCTEXT("VAT_TextureNormalToolTip", "Configure normal textures for vertex animation"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([]()
						{
							TArray<UTexture2D*> Textures;
							GetSelectedAssets<UTexture2D>(Textures);

							ConfigureTexturesForVertexAnim(Textures, EGamedevHelperVertexAnimTexture::Normal);
						})
					)
				);
				MenuBuilder.AddMenuEntry(
					LOCTEXT("VAT_TextureUV", "Configure as UV"),
					LOCTEXT("VAT_TextureUVToolTip", "Configure UV textures for vertex animation"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([]()
						{
							TArray<UTexture2D*> Textures;
							GetSelectedAssets<UTexture2D>(Textures);

							ConfigureTexturesForVertexAnim(Textures, EGamedevHelperVertexAnimTexture::UV);
						})
					)
				);
				MenuBuilder.EndSection();
			}),
			false,
			FSlateIcon(FGamedevHelperEditorStyle::GetStyleSetName(), "GamedevHelper.Icon16")
		);
	}

	// level sequence asset actions
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.LevelSequence");
		FToolMenuSection& Section = Menu->FindOrAddSection("GamedevHelperActions");

		Section.AddSubMenu(
			"GamedevHelperSubMenu",
			LOCTEXT("GamedevHelperSubMenu", "GamedevHelper Actions"),
			LOCTEXT("GamedevHelperSubMenu_ToolTip", "GamedevHelper plugin asset helper actions"),
			FNewMenuDelegate::CreateLambda([&](FMenuBuilder& MenuBuilder)
			{
				MenuBuilder.BeginSection("Section_Util", FText::FromString("Utility"));
				MenuBuilder.AddMenuEntry(
					LOCTEXT("Util_Render", "Render"),
					LOCTEXT("Util_Render_ToolTip", "Renders specified level sequences using movie render and then creates video files from them"),
					FSlateIcon(),
					FUIAction(
						FExecuteAction::CreateLambda([]()
						{
							// todo:ashe23 implement later
							UE_LOG(LogGamedevHelper, Warning, TEXT("Rendering level sequence"))
						})
					)
				);
				MenuBuilder.EndSection();
			}),
			false,
			FSlateIcon(FGamedevHelperEditorStyle::GetStyleSetName(), "GamedevHelper.Icon16")
		);
	}
}

template <class ClassType>
void UGamedevHelperSubsystem::GetSelectedAssets(TArray<ClassType*>& Assets)
{
	TArray<FAssetData> SelectedAssets;
	GetSelectedAssets(SelectedAssets);

	Assets.Reserve(SelectedAssets.Num());
	for (const auto& Asset : SelectedAssets)
	{
		ClassType* SpecificAsset = Cast<ClassType>(Asset.GetAsset());
		if (!SpecificAsset) continue;

		Assets.Add(SpecificAsset);
	}
}

SNotificationItem::ECompletionState UGamedevHelperSubsystem::GetCompletionStateFromModalStatus(const EGamedevHelperModalStatus ModalStatus)
{
	if (ModalStatus == EGamedevHelperModalStatus::Pending)
	{
		return SNotificationItem::CS_Pending;
	}
	if (ModalStatus == EGamedevHelperModalStatus::Fail)
	{
		return SNotificationItem::CS_Fail;
	}
	if (ModalStatus == EGamedevHelperModalStatus::Success)
	{
		return SNotificationItem::CS_Success;
	}

	return SNotificationItem::CS_None;
}

#undef LOCTEXT_NAMESPACE