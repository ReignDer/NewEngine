// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonSceneImporter.generated.h"

/**
 * 
 */
UCLASS()
class SCENEIMPORTER_API UJsonSceneImporter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// Call this from C++, Python, or an Editor Utility Widget
	UFUNCTION(BlueprintCallable, Category = "Scene Importer")
	static void ImportCustomSceneFromJSON(const FString& FilePath);

private:
	// Helper functions to convert JSON arrays to Unreal Math types
	static FVector ReadVectorFromArray(const TArray<TSharedPtr<FJsonValue>>& Array);
	static FRotator ReadRotatorFromArray(const TArray<TSharedPtr<FJsonValue>>& Array);
	static class UStaticMesh* GetEnginePrimitiveMesh(const FString& PrimitiveType);
};
