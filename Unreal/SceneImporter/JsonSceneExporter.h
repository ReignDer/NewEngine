// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonSceneExporter.generated.h"

/**
 * 
 */
UCLASS()
class SCENEIMPORTER_API UJsonSceneExporter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "JsonScene")
	static bool ExportCurrentLevelToJson(const FString& FilePath);
	UFUNCTION(BlueprintCallable, Category = "JsonScene")
	static bool ExportCurrentLevelToJsonWithDialog();

private:
	static FVector ConvertPositionBack(const FVector& UEPos);
	static FVector ConvertScaleBack(const FVector& UEScale);
	static FVector ConvertRotationBack(const FQuat& UEQuat);
};

