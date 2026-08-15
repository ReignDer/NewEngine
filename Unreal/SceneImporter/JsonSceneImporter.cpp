// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonSceneImporter.h"
#include "Editor.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/FileHelper.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

void UJsonSceneImporter::ImportCustomSceneFromJSON(const FString& FilePath)
{
	// 1. Read JSON file content
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("[SceneImporter] Failed to load file: %s"), *FilePath);
		return;
	}

	// 2. Deserialize JSON
	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[SceneImporter] Failed to parse JSON content."));
		return;
	}

	// 3. Get Active Editor World
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[SceneImporter] No active Editor World found."));
		return;
	}

	// 4. Iterate Entities Array
	const TArray<TSharedPtr<FJsonValue>>* EntitiesArray;
	if (RootObject->TryGetArrayField(TEXT("Entities"), EntitiesArray))
	{
		for (const TSharedPtr<FJsonValue>& EntityValue : *EntitiesArray)
		{
			TSharedPtr<FJsonObject> EntityObj = EntityValue->AsObject();
			if (!EntityObj.IsValid()) continue;

			// --- Parse Tag Component ---
			FString ActorLabel = TEXT("Entity");
			if (EntityObj->HasField(TEXT("TagComponent")))
			{
				TSharedPtr<FJsonObject> TagObj = EntityObj->GetObjectField(TEXT("TagComponent"));
				ActorLabel = TagObj->GetStringField(TEXT("Tag"));
			}

			// --- Parse Transform Component ---
			FVector Location = FVector::ZeroVector;
			FRotator Rotation = FRotator::ZeroRotator;
			FVector Scale = FVector::OneVector;

			if (EntityObj->HasField(TEXT("TransformComponent")))
			{
				TSharedPtr<FJsonObject> TransformObj = EntityObj->GetObjectField(TEXT("TransformComponent"));

				if (TransformObj->HasField(TEXT("Translation")))
				{
					// Convert coordinate axes if needed (e.g., scale JSON meters to UE centimeters)
					Location = ReadVectorFromArray(TransformObj->GetArrayField(TEXT("Translation"))) * 100.0f;
				}
				if (TransformObj->HasField(TEXT("Rotation")))
				{
					Rotation = ReadRotatorFromArray(TransformObj->GetArrayField(TEXT("Rotation")));
				}
				if (TransformObj->HasField(TEXT("Scale")))
				{
					Scale = ReadVectorFromArray(TransformObj->GetArrayField(TEXT("Scale")));
				}
			}

			// --- Parse MeshRenderer Component ---
			UStaticMesh* MeshToAssign = nullptr;
			if (EntityObj->HasField(TEXT("MeshRendererComponent")))
			{
				TSharedPtr<FJsonObject> MeshObj = EntityObj->GetObjectField(TEXT("MeshRendererComponent"));
				FString PrimitiveType = MeshObj->GetStringField(TEXT("PrimitiveType"));
				MeshToAssign = GetEnginePrimitiveMesh(PrimitiveType);
			}

			// --- Spawn Actor ---
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			FTransform SpawnTransform(Rotation, Location, Scale);
			AStaticMeshActor* NewActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnTransform, SpawnParams);

			if (NewActor)
			{
				NewActor->SetActorLabel(*ActorLabel);

				if (MeshToAssign)
				{
					NewActor->GetStaticMeshComponent()->SetStaticMesh(MeshToAssign);
				}

				// --- Optional Physics Type check ---
				if (EntityObj->HasField(TEXT("RigidBody3DComponent")))
				{
					TSharedPtr<FJsonObject> PhysicsObj = EntityObj->GetObjectField(TEXT("RigidBody3DComponent"));
					FString BodyType = PhysicsObj->GetStringField(TEXT("Type"));

					if (BodyType.Equals(TEXT("Dynamic"), ESearchCase::IgnoreCase))
					{
						NewActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
						NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
					}
					else
					{
						NewActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Static);
					}
				}

				World->MarkPackageDirty();
			}
		}
	}
}

// Array Parsing Helpers
FVector UJsonSceneImporter::ReadVectorFromArray(const TArray<TSharedPtr<FJsonValue>>& Array)
{
	if (Array.Num() >= 3)
	{
		return FVector(Array[0]->AsNumber(), Array[1]->AsNumber(), Array[2]->AsNumber());
	}
	return FVector::ZeroVector;
}

FRotator UJsonSceneImporter::ReadRotatorFromArray(const TArray<TSharedPtr<FJsonValue>>& Array)
{
	if (Array.Num() >= 3)
	{
		// Pitch (Y), Yaw (Z), Roll (X)
		return FRotator(Array[0]->AsNumber(), Array[1]->AsNumber(), Array[2]->AsNumber());
	}
	return FRotator::ZeroRotator;
}

// Maps primitive names to engine default static meshes
UStaticMesh* UJsonSceneImporter::GetEnginePrimitiveMesh(const FString& PrimitiveType)
{
	FString Path;
	if (PrimitiveType.Equals(TEXT("Cube"), ESearchCase::IgnoreCase))
	{
		Path = TEXT("/Engine/BasicShapes/Cube.Cube");
	}
	else if (PrimitiveType.Equals(TEXT("Plane"), ESearchCase::IgnoreCase))
	{
		Path = TEXT("/Engine/BasicShapes/Plane.Plane");
	}
	else if (PrimitiveType.Equals(TEXT("Sphere"), ESearchCase::IgnoreCase))
	{
		Path = TEXT("/Engine/BasicShapes/Sphere.Sphere");
	}
	else if (PrimitiveType.Equals(TEXT("Capsule"), ESearchCase::IgnoreCase))
	{
		Path = TEXT("/Engine/BasicShapes/Cylinder.Cylinder"); // UE uses Cylinder for basic shape
	}

	if (!Path.IsEmpty())
	{
		return Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *Path));
	}
	return nullptr;
}
