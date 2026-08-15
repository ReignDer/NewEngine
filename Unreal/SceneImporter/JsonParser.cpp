// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonParser.h"

UJsonParser::UJsonParser()
{
}

UJsonParser::~UJsonParser()
{
}

EInterchangeTranslatorType UJsonParser::GetTranslatorType() const
{
	return EInterchangeTranslatorType::Scenes;
}

EInterchangeTranslatorAssetType UJsonParser::GetSupportedAssetTypes() const
{
	return EInterchangeTranslatorAssetType::None | EInterchangeTranslatorAssetType::Meshes;
}

TArray<FString> UJsonParser::GetSupportedFormats() const
{
	UE_LOG(LogTemp, Warning, TEXT("JsonParser::GetSupportedFormats called"));
	TArray<FString> Formats{ TEXT("level;Custom Scene JSON") };
	return Formats;
}

bool UJsonParser::Translate(UInterchangeBaseNodeContainer& BaseNodeContainer) const
{
	FString JsonFilePath = GetSourceData()->GetFilename();
	FString JsonString;

	if (!FFileHelper::LoadFileToString(JsonString, *JsonFilePath))
	{
		return false;
	}

	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid()) {
		return false;
	}
	const TArray<TSharedPtr<FJsonValue>>* Entities;
	if (!RootObject->TryGetArrayField(TEXT("Entities"), Entities))
	{
		return false;
	}

	static const TMap<FString, FString> PrimitiveToEnginePrimitive =
	{
		{TEXT("Cube"), TEXT("/Engine/BasicShapes/Cube.Cube")},
		{TEXT("Sphere"), TEXT("/Engine/BasicShapes/Sphere.Sphere")},
		{TEXT("Plane"), TEXT("/Engine/BasicShapes/Plane.Plane")},
		{TEXT("Capsule"), TEXT("/Engine/BasicShapes/Cylinder.Cylinder")}
	};


	for (const TSharedPtr<FJsonValue>& EntityValue : *Entities)
	{
		const TSharedPtr<FJsonObject> EntityObj = EntityValue->AsObject();

		if (!EntityObj.IsValid())
		{
			return false;
		}

		FString UUIDString;
		if (!EntityObj->TryGetStringField(TEXT("Entity"), UUIDString))
		{
			UE_LOG(LogTemp, Warning, TEXT("JsonParser: entity missing UUID, skipping"));
			continue; 
		}
		uint64 EntityUUID = FCString::Strtoui64(*UUIDString, nullptr, 10);
		FString Tag = TEXT("ENTITY");
		const TSharedPtr<FJsonObject>* TagComp;

		if (EntityObj->TryGetObjectField(TEXT("TagComponent"), TagComp))
		{
			
			(*TagComp)->TryGetStringField(TEXT("Tag"), Tag);
		}

		const FString NodeUID = FString::Printf(TEXT("\\Scene\\%s_%lld"), *Tag, EntityUUID);
		UInterchangeSceneNode* SceneNode = NewObject<UInterchangeSceneNode>(&BaseNodeContainer);
		SceneNode->InitializeNode(NodeUID, Tag, EInterchangeNodeContainerType::TranslatedScene);
		BaseNodeContainer.AddNode(SceneNode);

		const TSharedPtr<FJsonObject>* TransformComponent;
		if (EntityObj->TryGetObjectField(TEXT("TransformComponent"), TransformComponent))
		{
			const TSharedPtr<FJsonObject>* MeshComponent;
			if (EntityObj->TryGetObjectField(TEXT("MeshRendererComponent"), MeshComponent))
			{
				FString PrimitiveType;
				(*MeshComponent)->TryGetStringField(TEXT("PrimitiveType"), PrimitiveType);
				FVector Translation = ReadVec3(*TransformComponent, TEXT("Translation"));
				FVector Rotation = ReadVec3(*TransformComponent, TEXT("Rotation"));
				FVector Scale = ReadVec3(*TransformComponent, TEXT("Scale"));
				FVector FinalScale = ConvertScale(Scale);
				if (PrimitiveType == "Plane")
				{
					FinalScale.X *= 10.f;
					FinalScale.Y *= 10.f;
				}

				FTransform LocalTransform;
				LocalTransform.SetTranslation(ConvertPosition(Translation));
				LocalTransform.SetRotation(ConvertRotation(Rotation));
				LocalTransform.SetScale3D(FinalScale);

				SceneNode->SetCustomLocalTransform(&BaseNodeContainer, LocalTransform);
			}
		}

		const TSharedPtr<FJsonObject>* MeshComponent;
		if (EntityObj->TryGetObjectField(TEXT("MeshRendererComponent"), MeshComponent))
		{
			FString PrimitiveType;

			(*MeshComponent)->TryGetStringField(TEXT("PrimitiveType"), PrimitiveType);
			SceneNode->AddStringAttribute(TEXT("PrimitiveType"), PrimitiveType);

			if (const FString* EnginePath = PrimitiveToEnginePrimitive.Find(PrimitiveType))
			{
				SceneNode->AddStringAttribute(TEXT("CustomPrimitivePath"), *EnginePath);
			}
		}

		const TSharedPtr<FJsonObject>* RigidbodyComponent;
		if (EntityObj->TryGetObjectField(TEXT("RigidBody3DComponent"), RigidbodyComponent))
		{
			FString BodyType;
			(*RigidbodyComponent)->TryGetStringField(TEXT("Type"), BodyType);
			SceneNode->AddBooleanAttribute(TEXT("Physics_HasRigidbody"), true);
			SceneNode->AddStringAttribute(TEXT("Physics_RigidbodyType"), BodyType);
		}
		else
		{
			SceneNode->AddBooleanAttribute(TEXT("Physics_HasRigidbody"), false);
		}

	}
	TArray<FString> SceneNodeUids;
	BaseNodeContainer.GetNodes(UInterchangeSceneNode::StaticClass(), SceneNodeUids);
	UE_LOG(LogTemp, Warning, TEXT("JsonParser: Created %d scene nodes"), SceneNodeUids.Num());
	return true;


}

void UJsonParser::WriteVec3Attribute(UInterchangeSceneNode* SceneNode, const FString& AttributePrefix, const FVector& Value)
{
	SceneNode->AddFloatAttribute(AttributePrefix + TEXT("_X"), Value.X);
	SceneNode->AddFloatAttribute(AttributePrefix + TEXT("_Y"), Value.Y);
	SceneNode->AddFloatAttribute(AttributePrefix + TEXT("_Z"), Value.Z);
}

void UJsonParser::WriteColliderAttribute(UInterchangeSceneNode* SceneNode, const TSharedPtr<FJsonObject>& EntityObj, const FString& ComponentKey, const FString& ColliderTypeName)
{
	const TSharedPtr<FJsonObject>* ColliderComponent;
	if (!EntityObj->TryGetObjectField(ComponentKey, ColliderComponent))
	{
		return;
	}

	SceneNode->AddStringAttribute(TEXT("Physics_ColliderType"), ColliderTypeName);

	double Density = 0.0;
	double Friction = 0.0;
	double Restitution = 0.0;

	(*ColliderComponent)->TryGetNumberField(TEXT("Density"), Density);
	(*ColliderComponent)->TryGetNumberField(TEXT("Friction"), Friction);
	(*ColliderComponent)->TryGetNumberField(TEXT("Restitution"), Restitution);

	SceneNode->AddFloatAttribute(TEXT("Physics_Density"), Density);
	SceneNode->AddFloatAttribute(TEXT("Physics_Friction"), Friction);
	SceneNode->AddFloatAttribute(TEXT("Physics_Restitution"), Restitution);

	FVector Offset = ConvertPosition(ReadVec3(*ColliderComponent, TEXT("Offset")));
	WriteVec3Attribute(SceneNode, TEXT("Physics_ColliderOffset"), Offset);

	if (ColliderTypeName == TEXT("Box"))
	{
		FVector Size = ConvertScale(ReadVec3(*ColliderComponent, TEXT("Size")));
		WriteVec3Attribute(SceneNode, TEXT("PhysicsColliderSize"), Size);
	}
	else if (ColliderTypeName == TEXT("Sphere"))
	{
		double Radius = 0.0;
		(*ColliderComponent)->TryGetNumberField(TEXT("Radius"), Radius);
		SceneNode->AddFloatAttribute(TEXT("Physics_ColliderRadius"), Radius);
	}
	else if (ColliderTypeName == TEXT("Capsule"))
	{
		double Radius = 0.0, Height = 0.0;
		(*ColliderComponent)->TryGetNumberField(TEXT("Radius"), Radius);
		(*ColliderComponent)->TryGetNumberField(TEXT("Height"), Height);
		SceneNode->AddFloatAttribute(TEXT("Physics_ColliderRadius"), Radius);
		SceneNode->AddFloatAttribute(TEXT("Physics_ColliderHeight"), Height);
	}
}
