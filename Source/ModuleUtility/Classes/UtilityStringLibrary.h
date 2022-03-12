﻿// Copyright Ashot Barkhudaryan. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UtilityStringLibrary.generated.h"

UCLASS()
class UGamedevHelperStringLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief Returns first letter of given string
	 * - "abc" => "a"
	 * - "" => ""
	 * @param OriginalString FString
	 * @return FString
	 */
	UFUNCTION(BlueprintCallable, Category="GamedevHelper|String")
	static FString GetFirstLetter(const FString& OriginalString);

	/**
	 * @brief Returns last letter of given string
	 * - "abc" => "c"
	 * - "" => ""
	 * @param OriginalString FString
	 * @return FString
	 */
	UFUNCTION(BlueprintCallable, Category="GamedevHelper|String")
	static FString GetLastLetter(const FString& OriginalString);


	/**
	 * @brief Removes first letter of given string
	 * @param OriginalString FString
	 * @return FString
	 */
	UFUNCTION(BlueprintCallable, Category="GamedevHelper|String")
	static FString RemoveFirstLetter(const FString& OriginalString);

	/**
	 * @brief Removes last letter of given string
	 * @param OriginalString FString
	 * @return FString
	 */
	UFUNCTION(BlueprintCallable, Category="GamedevHelper|String")
	static FString RemoveLastLetter(const FString& OriginalString);
};