// Copyright Epic Games, Inc. All Rights Reserved.

#include "SceneImporter.h"
#include "InterchangeManager.h"
#include "JsonParser.h"



void FSceneImporterModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([]()
		{
			UInterchangeManager& InterchangeManager = UInterchangeManager::GetInterchangeManager();
			InterchangeManager.RegisterTranslator(UJsonParser::StaticClass());
		});
}

void FSceneImporterModule::ShutdownModule()
{
}

IMPLEMENT_PRIMARY_GAME_MODULE(FSceneImporterModule, SceneImporter, "SceneImporter");