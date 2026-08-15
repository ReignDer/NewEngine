// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonSceneExporter.h"
#include "Engine/World.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMesh.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/FileHelper.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"

static constexpr float UEUnitsToSource = 1.0f / 100.0f;

FVector UJsonSceneExporter::ConvertPositionBack(const FVector& UEPos)
{

	return FVector(-UEPos.X, UEPos.Z, UEPos.Y) * UEUnitsToSource;
}

FVector UJsonSceneExporter::ConvertScaleBack(const FVector& UEScale)
{
	return FVector(UEScale.X, UEScale.Z, UEScale.Y);
}

FVector UJsonSceneExporter::ConvertRotationBack(const FQuat& UEQuat)
{
	FQuat SourceQuat(UEQuat.X, UEQuat.Z, UEQuat.Y, -UEQuat.W);
	SourceQuat.Normalize();
	return SourceQuat.Euler();
}

static FString GuessPrimitiveTypeFromMesh(UStaticMesh* Mesh)
{
	if (!Mesh)
	{
		return TEXT("");
	}
	FString Name = Mesh->GetName();
	if (Name.Contains(TEXT("Cube")))    return TEXT("Cube");
	if (Name.Contains(TEXT("Sphere")))  return TEXT("Sphere");
	if (Name.Contains(TEXT("Plane")))   return TEXT("Plane");
	if (Name.Contains(TEXT("Cylinder")))return TEXT("Capsule");
	return TEXT("");
}

bool UJsonSceneExporter::ExportCurrentLevelToJson(const FString& FilePath)
{
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("JsonSceneExporter: no editor world found"));
		return false;
	}

	TSharedPtr<FJsonObject> RootObject = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> EntitiesArray;

	int32 EntityIdCounter = 60000; 

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
		if (!MeshComp)
		{
			continue;
		}

		TSharedPtr<FJsonObject> EntityObj = MakeShared<FJsonObject>();
		EntityObj->SetNumberField(TEXT("Entity"), EntityIdCounter++);

		TSharedPtr<FJsonObject> TagObj = MakeShared<FJsonObject>();
		TagObj->SetStringField(TEXT("Tag"), Actor->GetActorLabel());
		EntityObj->SetObjectField(TEXT("TagComponent"), TagObj);

		{
			FString PrimitiveType = GuessPrimitiveTypeFromMesh(MeshComp->GetStaticMesh());
			if (!PrimitiveType.IsEmpty())
			{
			
				FTransform T = Actor->GetActorTransform();
				FVector Scale;
				if (PrimitiveType == "Plane")
				{
					Scale = T.GetScale3D();
					Scale.X /= 10.f;
					Scale.Y /= 10.f;
				}
				else
				{
					Scale = T.GetScale3D();
				}
				Scale = ConvertScaleBack(Scale);
				FVector Pos = ConvertPositionBack(T.GetLocation());
				FVector Rot = ConvertRotationBack(T.GetRotation());

				auto MakeVecArray = [](const FVector& V)
					{
						TArray<TSharedPtr<FJsonValue>> Arr;
						Arr.Add(MakeShared<FJsonValueNumber>(V.X));
						Arr.Add(MakeShared<FJsonValueNumber>(V.Y));
						Arr.Add(MakeShared<FJsonValueNumber>(V.Z));
						return Arr;
					};

				TSharedPtr<FJsonObject> XformObj = MakeShared<FJsonObject>();
				XformObj->SetArrayField(TEXT("Translation"), MakeVecArray(Pos));
				XformObj->SetArrayField(TEXT("Rotation"), MakeVecArray(Rot));
				XformObj->SetArrayField(TEXT("Scale"), MakeVecArray(Scale));
				EntityObj->SetObjectField(TEXT("TransformComponent"), XformObj);
			}
		}

		{
			FString PrimitiveType = GuessPrimitiveTypeFromMesh(MeshComp->GetStaticMesh());
			if (!PrimitiveType.IsEmpty())
			{
				TSharedPtr<FJsonObject> MeshObj = MakeShared<FJsonObject>();
				MeshObj->SetStringField(TEXT("PrimitiveType"), PrimitiveType);
				MeshObj->SetStringField(TEXT("SourceType"), TEXT("Primitive"));
				EntityObj->SetObjectField(TEXT("MeshRendererComponent"), MeshObj);
			}
		}

		if (MeshComp->IsSimulatingPhysics())
		{
			TSharedPtr<FJsonObject> RBObj = MakeShared<FJsonObject>();
			RBObj->SetStringField(TEXT("Type"), TEXT("Dynamic"));
			EntityObj->SetObjectField(TEXT("RigidBody3DComponent"), RBObj);
		}
		else if (MeshComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
		{
			TSharedPtr<FJsonObject> RBObj = MakeShared<FJsonObject>();
			RBObj->SetStringField(TEXT("Type"), TEXT("Static"));
			EntityObj->SetObjectField(TEXT("RigidBody3DComponent"), RBObj);
		}

		if (UBoxComponent* Box = Actor->FindComponentByClass<UBoxComponent>())
		{
			FVector Size = ConvertScaleBack(Box->GetUnscaledBoxExtent() * 2.0f) * UEUnitsToSource;
			TSharedPtr<FJsonObject> ColliderObj = MakeShared<FJsonObject>();
			ColliderObj->SetArrayField(TEXT("Size"), { MakeShared<FJsonValueNumber>(Size.X), MakeShared<FJsonValueNumber>(Size.Y), MakeShared<FJsonValueNumber>(Size.Z) });
			ColliderObj->SetNumberField(TEXT("Density"), 1.0);
			ColliderObj->SetNumberField(TEXT("Friction"), 0.5);
			ColliderObj->SetNumberField(TEXT("Restitution"), 0.0);
			EntityObj->SetObjectField(TEXT("Box3DColliderComponent"), ColliderObj);
		}
		else if (USphereComponent* Sphere = Actor->FindComponentByClass<USphereComponent>())
		{
			TSharedPtr<FJsonObject> ColliderObj = MakeShared<FJsonObject>();
			ColliderObj->SetNumberField(TEXT("Radius"), Sphere->GetUnscaledSphereRadius() * UEUnitsToSource);
			ColliderObj->SetNumberField(TEXT("Density"), 1.0);
			ColliderObj->SetNumberField(TEXT("Friction"), 0.5);
			ColliderObj->SetNumberField(TEXT("Restitution"), 0.0);
			EntityObj->SetObjectField(TEXT("SphereColliderComponent"), ColliderObj);
		}
		else if (UCapsuleComponent* Capsule = Actor->FindComponentByClass<UCapsuleComponent>())
		{
			TSharedPtr<FJsonObject> ColliderObj = MakeShared<FJsonObject>();
			ColliderObj->SetNumberField(TEXT("Radius"), Capsule->GetUnscaledCapsuleRadius() * UEUnitsToSource);
			ColliderObj->SetNumberField(TEXT("Height"), Capsule->GetUnscaledCapsuleHalfHeight() * 2.0f * UEUnitsToSource);
			ColliderObj->SetNumberField(TEXT("Density"), 1.0);
			ColliderObj->SetNumberField(TEXT("Friction"), 0.5);
			ColliderObj->SetNumberField(TEXT("Restitution"), 0.0);
			EntityObj->SetObjectField(TEXT("CapsuleColliderComponent"), ColliderObj);
		}

		EntitiesArray.Add(MakeShared<FJsonValueObject>(EntityObj));
	}

	RootObject->SetArrayField(TEXT("Entities"), EntitiesArray);
	RootObject->SetStringField(TEXT("Scene"), FPaths::GetBaseFilename(FilePath));

	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

	return FFileHelper::SaveStringToFile(OutputString, *FilePath);
}

bool UJsonSceneExporter::ExportCurrentLevelToJsonWithDialog()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		return false;
	}

	const void* ParentWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

	TArray<FString> OutFilenames;
	const bool bSaved = DesktopPlatform->SaveFileDialog(
		ParentWindowHandle,
		TEXT("Export Scene"),
		TEXT(""),                 // default path
		TEXT("scene.level"),       // default filename
		TEXT("JSON Files (*.level)|*.level"),
		EFileDialogFlags::None,
		OutFilenames
	);

	if (!bSaved || OutFilenames.Num() == 0)
	{
		return false;
	}

	return ExportCurrentLevelToJson(OutFilenames[0]);
}
