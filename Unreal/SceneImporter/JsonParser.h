// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InterchangeTranslatorBase.h"
#include "Nodes/InterchangeBaseNodeContainer.h"
#include "InterchangeSceneNode.h"
#include "Dom/JsonObject.h"
#include "JsonParser.generated.h"
/**
 * 
 */
UCLASS()
class SCENEIMPORTER_API UJsonParser : public UInterchangeTranslatorBase
{
	GENERATED_BODY()
public:
	UJsonParser();
	~UJsonParser();
	virtual EInterchangeTranslatorType GetTranslatorType() const override;
	virtual EInterchangeTranslatorAssetType GetSupportedAssetTypes() const override;
	virtual TArray<FString> GetSupportedFormats() const override;
	virtual bool Translate(UInterchangeBaseNodeContainer& BaseNodeContainer) const override;

private:
	static void WriteVec3Attribute(UInterchangeSceneNode* SceneNode, const FString& AttributePrefix, const FVector& Value);
	static void WriteColliderAttribute(UInterchangeSceneNode* SceneNode, const TSharedPtr<FJsonObject>& EntityObj, const FString& ComponentKey, const FString& ColliderTypeName);
	static FVector ConvertPosition(const FVector& sourceUp)
	{
		return FVector(-sourceUp.X * 100.f, sourceUp.Z * 100.f, sourceUp.Y * 100.f);
	}
	static FVector ConvertScale(const FVector& sourceUp)
	{
		return FVector(sourceUp.X, sourceUp.Z, sourceUp.Y);
	}
	static FQuat ConvertRotation(const FVector& sourceUp)
	{
		FQuat Source = FQuat::MakeFromEuler(sourceUp);
		FQuat Converted(Source.X, Source.Z, Source.Y, -Source.W);
		Converted.Normalize();
		return Converted;
	}
	static FVector ReadVec3(const TSharedPtr<FJsonObject>& jsonObject, const FString& fieldName, const FVector& vector = FVector::Zero())
	{
		if (!jsonObject.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("JsonParser: No Field Name"));
			return vector;
		}

		const TArray<TSharedPtr<FJsonValue>>* Array;

		if (!jsonObject->TryGetArrayField(fieldName, Array))
		{
			UE_LOG(LogTemp, Warning, TEXT("JsonParser: No Field Name"));
			return vector;
		}
	
		return FVector(
			(*Array)[0]->AsNumber(),
			(*Array)[1]->AsNumber(),
			(*Array)[2]->AsNumber()
		);
	}

};
