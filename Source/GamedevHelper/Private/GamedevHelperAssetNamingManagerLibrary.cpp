﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#include "GamedevHelperAssetNamingManagerLibrary.h"
#include "GamedevHelper.h"
#include "GamedevHelperStringLibrary.h"
#include "GamedevHelperAssetLibrary.h"
// Engine Headers
#include "Kismet/KismetStringLibrary.h"

FString UGamedevHelperAssetNamingManagerLibrary::GetRenamedName(const FAssetData& Asset, const UGamedevHelperAssetNamingManagerSettings* Settings)
{
	if (!Asset.IsValid())
	{
		UE_LOG(LogGamedevHelper, Error, TEXT("Invalid asset"));
		return FString{};
	}
	if (!Settings)
	{
		UE_LOG(LogGamedevHelper, Warning, TEXT("Invalid Asset naming settings"));
		return FString{};
	}

	const FString AssetOldName = Asset.AssetName.ToString();

	const FGamedevHelperAssetNameSettings* AssetNameSettings = GetAssetNamingSettings(Asset, Settings);
	if (!AssetNameSettings)
	{
		UE_LOG(LogGamedevHelper, Warning, TEXT("Cant find asset name settings for %s"), *AssetOldName);
		return FString{};
	}

	FString AssetNewName = ConvertNamingCase(AssetOldName, Settings->NamingCase);

	if (!AssetOldName.StartsWith(AssetNameSettings->Prefix))
	{
		const FString NameCaseFixedPrefix = Settings->bIgnoreNamingCaseOnPrefixes
			                                    ? AssetNameSettings->Prefix
			                                    : ConvertNamingCase(AssetNameSettings->Prefix, Settings->NamingCase);

		if (!NameCaseFixedPrefix.IsEmpty())
		{
			AssetNewName.InsertAt(0, NameCaseFixedPrefix + TEXT("_"));
		}
	}

	if (!AssetOldName.EndsWith(AssetNameSettings->Suffix))
	{
		const FString NameCaseFixedSuffix = Settings->bIgnoreNamingCaseOnSuffixes
			                                    ? AssetNameSettings->Suffix
			                                    : ConvertNamingCase(AssetNameSettings->Suffix, Settings->NamingCase);

		if (!NameCaseFixedSuffix.IsEmpty())
		{
			AssetNewName.Append(TEXT("_") + NameCaseFixedSuffix);
		}
	}

	return AssetNewName;
}

void UGamedevHelperAssetNamingManagerLibrary::RenameAsset(const FAssetData& Asset, const UGamedevHelperAssetNamingManagerSettings* Settings)
{
	// todo:ashe23
}

void UGamedevHelperAssetNamingManagerLibrary::RenameAssets(const TArray<FAssetData>& Assets, const UGamedevHelperAssetNamingManagerSettings* Settings)
{
	// todo:ashe23
}

const FGamedevHelperAssetNameSettings* UGamedevHelperAssetNamingManagerLibrary::GetAssetNamingSettings(
	const FAssetData& Asset,
	const UGamedevHelperAssetNamingManagerSettings* Settings)
{
	if (!Asset.IsValid()) return nullptr;
	if (!Settings) return nullptr;

	const auto AssetNaming = Settings->AssetsNaming.Find(Asset.GetClass());
	if (AssetNaming)
	{
		return AssetNaming;
	}

	if (UGamedevHelperAssetLibrary::IsBlueprint(Asset))
	{
		return Settings->BlueprintTypesNaming.Find(UGamedevHelperAssetLibrary::GetBlueprintType(Asset));
	}

	return nullptr;
}

FString UGamedevHelperAssetNamingManagerLibrary::Normalize(const FString& OriginalString)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	FString CleanedString;
	CleanedString.Reserve(OriginalString.Len());

	const auto Chars = OriginalString.GetCharArray();
	for (const auto Char : Chars)
	{
		if (FChar::IsAlnum(Char) || FChar::IsDigit(Char))
		{
			CleanedString.AppendChar(Char);
		}
		else
		{
			CleanedString.AppendChar('_');
		}
	}

	TArray<FString> Parts;
	CleanedString.ParseIntoArray(Parts, TEXT("_"), true);

	return UKismetStringLibrary::JoinStringArray(Parts, TEXT("_"));
}

FString UGamedevHelperAssetNamingManagerLibrary::Tokenize(const FString& OriginalString)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	const FString Normalized = Normalize(OriginalString);


	FString Token;
	Token.AppendChar(Normalized[0]);
	const auto Chars = Normalized.GetCharArray();

	TArray<FString> Tokens;

	// todo:ash23 keep trailing Upper case letters as separate token
	for (int32 i = 1; i < Chars.Num() - 1; ++i)
	{
		const auto CurrentChar = Chars[i];
		const auto PreviousChar = Chars[i - 1];

		if (FChar::IsUnderscore(CurrentChar))
		{
			continue;
		}

		if (
			!FChar::IsUnderscore(CurrentChar) && FChar::IsUnderscore(PreviousChar) ||
			FChar::IsLower(CurrentChar) && FChar::IsDigit(PreviousChar) ||
			FChar::IsUpper(CurrentChar) && FChar::IsLower(PreviousChar) ||
			FChar::IsUpper(CurrentChar) && FChar::IsDigit(PreviousChar) ||
			FChar::IsDigit(CurrentChar) && FChar::IsLower(PreviousChar) ||
			FChar::IsDigit(CurrentChar) && FChar::IsUpper(PreviousChar)
		)
		{
			Tokens.Add(Token.ToLower());
			Token.Empty();
		}

		Token.AppendChar(CurrentChar);
	}

	Tokens.Add(Token.ToLower());

	return UKismetStringLibrary::JoinStringArray(Tokens, TEXT("_"));
}

FString UGamedevHelperAssetNamingManagerLibrary::ConvertNamingCase(const FString& OriginalString, const EGamedevHelperNamingCase NamingCase)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	if (NamingCase == EGamedevHelperNamingCase::CamelCase)
	{
		return ConvertToCamelCase(OriginalString);
	}

	if (NamingCase == EGamedevHelperNamingCase::KebabCase)
	{
		return ConvertToKebabCase(OriginalString);
	}

	if (NamingCase == EGamedevHelperNamingCase::PascalCase)
	{
		return ConvertToPascalCase(OriginalString);
	}

	if (NamingCase == EGamedevHelperNamingCase::SnakeCase)
	{
		return ConvertToSnakeCase(OriginalString);
	}

	if (NamingCase == EGamedevHelperNamingCase::PascalSnakeCase)
	{
		return ConvertToPascalSnakeCase(OriginalString);
	}

	return FString{};
}

FString UGamedevHelperAssetNamingManagerLibrary::ConvertToPascalCase(const FString& OriginalString)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	const FString Tokenized = Tokenize(OriginalString);
	TArray<FString> Parts;
	Tokenized.ParseIntoArray(Parts, TEXT("_"));

	TArray<FString> CapitalizedParts;
	CapitalizedParts.Reserve(Parts.Num());

	for (const auto& Part : Parts)
	{
		const FString FirstLetter = UKismetStringLibrary::GetSubstring(Part, 0, 1).ToUpper();
		const FString RestOfStr = UKismetStringLibrary::GetSubstring(Part, 1, Part.Len() - 1).ToLower();
		CapitalizedParts.Add(FirstLetter + RestOfStr);
	}

	return UKismetStringLibrary::JoinStringArray(CapitalizedParts, TEXT(""));
}

FString UGamedevHelperAssetNamingManagerLibrary::ConvertToPascalSnakeCase(const FString& OriginalString)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	const FString Tokenized = Tokenize(OriginalString);
	TArray<FString> Parts;
	Tokenized.ParseIntoArray(Parts, TEXT("_"));

	TArray<FString> CapitalizedParts;
	CapitalizedParts.Reserve(Parts.Num());

	for (const auto& Part : Parts)
	{
		const FString FirstLetter = UKismetStringLibrary::GetSubstring(Part, 0, 1).ToUpper();
		const FString RestOfStr = UKismetStringLibrary::GetSubstring(Part, 1, Part.Len() - 1).ToLower();
		CapitalizedParts.Add(FirstLetter + RestOfStr);
	}

	return UKismetStringLibrary::JoinStringArray(CapitalizedParts, TEXT("_"));
}

FString UGamedevHelperAssetNamingManagerLibrary::ConvertToSnakeCase(const FString& OriginalString)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	return Tokenize(OriginalString);
}

FString UGamedevHelperAssetNamingManagerLibrary::ConvertToKebabCase(const FString& OriginalString)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	const FString Tokenized = Tokenize(OriginalString);
	TArray<FString> Parts;
	Tokenized.ParseIntoArray(Parts, TEXT("_"));

	return UKismetStringLibrary::JoinStringArray(Parts, TEXT("-"));
}

FString UGamedevHelperAssetNamingManagerLibrary::ConvertToCamelCase(const FString& OriginalString)
{
	if (OriginalString.IsEmpty()) return OriginalString;

	const FString Tokenized = Tokenize(OriginalString);
	TArray<FString> Parts;
	Tokenized.ParseIntoArray(Parts, TEXT("_"));

	TArray<FString> CapitalizedParts;
	CapitalizedParts.Reserve(Parts.Num());
	CapitalizedParts.Add(Parts[0]);

	for (int32 i = 1; i < Parts.Num(); ++i)
	{
		const FString FirstLetter = UKismetStringLibrary::GetSubstring(Parts[i], 0, 1).ToUpper();
		const FString RestOfStr = UKismetStringLibrary::GetSubstring(Parts[i], 1, Parts[i].Len() - 1).ToLower();
		CapitalizedParts.Add(FirstLetter + RestOfStr);
	}

	return UKismetStringLibrary::JoinStringArray(CapitalizedParts, TEXT(""));
}
