// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InterchangeGenericScenesPipeline.h"
#include "JsonPipeline.generated.h"

/**
 * 
 */
UCLASS()
class SCENEIMPORTER_API UJsonPipeline : public UInterchangePipelineBase
{
	GENERATED_BODY()
public:

	virtual void ExecutePostFactoryPipeline(const UInterchangeBaseNodeContainer* InBaseNodeContainer, const FString& NodeKey, UObject* CreatedAsset, bool bIsAReimport) override;
private:
	void ApplyPhysics(const UInterchangeSceneNode* SceneNode, UPrimitiveComponent* PrimComp) const;
};
