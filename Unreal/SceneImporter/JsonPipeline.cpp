// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonPipeline.h"
#include "Nodes/InterchangeBaseNodeContainer.h"
#include "InterchangeActorFactoryNode.h"
#include "InterchangeSceneNode.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/PrimitiveComponent.h"
#include "Components/ShapeComponent.h"
#include "PhysicsEngine/BodySetup.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"

void UJsonPipeline::ExecutePostFactoryPipeline(
    const UInterchangeBaseNodeContainer* InBaseNodeContainer,
    const FString& NodeKey,
    UObject* CreatedAsset,
    bool bIsAReimport)
{
    Super::ExecutePostFactoryPipeline(InBaseNodeContainer, NodeKey, CreatedAsset, bIsAReimport);

    if (!InBaseNodeContainer || !CreatedAsset)
    {
        return;
    }

    AActor* TargetActor = Cast<AActor>(CreatedAsset);
    if (!TargetActor)
    {
        return;
    }

    const UInterchangeActorFactoryNode* ActorFactoryNode =
        Cast<UInterchangeActorFactoryNode>(InBaseNodeContainer->GetFactoryNode(NodeKey));
    if (!ActorFactoryNode)
    {
        return;
    }

    // The generic level pipeline links factory node -> original scene node via
    // AddTargetNodeUid(SceneNode->GetUniqueID()) even when TranslatedAssetNode was null.
    TArray<FString> TargetNodeUids;
    ActorFactoryNode->GetTargetNodeUids(TargetNodeUids);
    if (TargetNodeUids.Num() == 0)
    {
        return;
    }

    const UInterchangeSceneNode* SceneNode =
        Cast<UInterchangeSceneNode>(InBaseNodeContainer->GetNode(TargetNodeUids[0]));
    if (!SceneNode)
    {
        return;
    }

    FString MeshPath;
    if (!SceneNode->GetStringAttribute(TEXT("CustomPrimitivePath"), MeshPath)) // <-- corrected key
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("JsonPipeline: applying mesh %s to %s"), *MeshPath, *TargetActor->GetName());

    UStaticMesh* LoadedMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *MeshPath));
    if (!LoadedMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("JsonPipeline: failed to load mesh at %s"), *MeshPath);
        return;
    }

    UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(TargetActor, TEXT("StaticMeshComp"));
    if (!NewComponent)
    {
        return;
    }

    NewComponent->SetStaticMesh(LoadedMesh);
    NewComponent->SetMobility(EComponentMobility::Movable);

    if (!TargetActor->GetRootComponent())
    {
        TargetActor->SetRootComponent(NewComponent);
    }
    else
    {
        NewComponent->AttachToComponent(TargetActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
    }

    NewComponent->RegisterComponent();
    ApplyPhysics(SceneNode, NewComponent);
    TargetActor->AddInstanceComponent(NewComponent);
}

void UJsonPipeline::ApplyPhysics(const UInterchangeSceneNode* SceneNode, UPrimitiveComponent* PrimComp) const
{
    if (!SceneNode || !PrimComp)
    {
        return;
    }

    bool bHasRigidBody = false;
    SceneNode->GetBooleanAttribute(TEXT("Physics_HasRigidbody"), bHasRigidBody);

    FString RigidBodyType;
    SceneNode->GetStringAttribute(TEXT("Physics_RigidbodyType"), RigidBodyType);

    const bool bIsDynamic = bHasRigidBody && RigidBodyType == TEXT("Dynamic");

    PrimComp->SetSimulatePhysics(bIsDynamic);
    PrimComp->SetMobility(bIsDynamic ? EComponentMobility::Movable : EComponentMobility::Static);
    PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    float Density = 1.0f, Friction = 0.5f, Restitution = 0.0f;
    SceneNode->GetFloatAttribute(TEXT("Physics_Density"), Density);
    SceneNode->GetFloatAttribute(TEXT("Physics_Friction"), Friction);
    SceneNode->GetFloatAttribute(TEXT("Physics_Restitution"), Restitution);

    if (UBodySetup* BodySetup = PrimComp->GetBodySetup())
    {
        BodySetup->DefaultInstance.SetMassOverride(Density, false);

        if (UPhysicalMaterial* PhysMat = BodySetup->PhysMaterial)
        {
            // If you have a shared physical material asset, set Friction/Restitution
            // on it here. Since these values differ per-entity in your JSON, an
            // instance-level override on the component is more accurate than
            // mutating a shared PhysicalMaterial asset (which would affect every
            // actor using it). Consider creating a transient UPhysicalMaterial
            // per unique Friction/Restitution combo if per-entity accuracy matters.
        }
    }

    // --- Collider shape/size ---
    FString ColliderType;
    if (!SceneNode->GetStringAttribute(TEXT("Physics_ColliderType"), ColliderType) || ColliderType.IsEmpty())
    {
        return; // no collider component (matches entities like your sample's plain spheres/planes with only render mesh)
    }

    float OffsetX = 0.f, OffsetY = 0.f, OffsetZ = 0.f;
    SceneNode->GetFloatAttribute(TEXT("Physics_ColliderOffset_X"), OffsetX);
    SceneNode->GetFloatAttribute(TEXT("Physics_ColliderOffset_Y"), OffsetY);
    SceneNode->GetFloatAttribute(TEXT("Physics_ColliderOffset_Z"), OffsetZ);
    const FVector ColliderOffset(OffsetX, OffsetY, OffsetZ);

    AActor* Owner = PrimComp->GetOwner();
    if (!Owner)
    {
        return;
    }

    if (ColliderType == TEXT("Box"))
    {
        float SizeX = 100.f, SizeY = 100.f, SizeZ = 100.f;
        SceneNode->GetFloatAttribute(TEXT("Physics_ColliderSize_X"), SizeX);
        SceneNode->GetFloatAttribute(TEXT("Physics_ColliderSize_Y"), SizeY);
        SceneNode->GetFloatAttribute(TEXT("Physics_ColliderSize_Z"), SizeZ);

        UBoxComponent* BoxComp = NewObject<UBoxComponent>(Owner, TEXT("ColliderBox"));
        BoxComp->SetBoxExtent(FVector(SizeX, SizeY, SizeZ) * 0.5f); // extent is half-size
        BoxComp->SetRelativeLocation(ColliderOffset);
        BoxComp->AttachToComponent(PrimComp, FAttachmentTransformRules::KeepRelativeTransform);
        BoxComp->RegisterComponent();
        Owner->AddInstanceComponent(BoxComp);
    }
    else if (ColliderType == TEXT("Sphere"))
    {
        float Radius = 50.f;
        SceneNode->GetFloatAttribute(TEXT("Physics_ColliderRadius"), Radius);

        USphereComponent* SphereComp = NewObject<USphereComponent>(Owner, TEXT("ColliderSphere"));
        SphereComp->SetSphereRadius(Radius);
        SphereComp->SetRelativeLocation(ColliderOffset);
        SphereComp->AttachToComponent(PrimComp, FAttachmentTransformRules::KeepRelativeTransform);
        SphereComp->RegisterComponent();
        Owner->AddInstanceComponent(SphereComp);
    }
    else if (ColliderType == TEXT("Capsule"))
    {
        float Radius = 50.f, Height = 200.f;
        SceneNode->GetFloatAttribute(TEXT("Physics_ColliderRadius"), Radius);
        SceneNode->GetFloatAttribute(TEXT("Physics_ColliderHeight"), Height);

        UCapsuleComponent* CapsuleComp = NewObject<UCapsuleComponent>(Owner, TEXT("ColliderCapsule"));
        CapsuleComp->SetCapsuleSize(Radius, Height * 0.5f); // half-height
        CapsuleComp->SetRelativeLocation(ColliderOffset);
        CapsuleComp->AttachToComponent(PrimComp, FAttachmentTransformRules::KeepRelativeTransform);
        CapsuleComp->RegisterComponent();
        Owner->AddInstanceComponent(CapsuleComp);
    }
}