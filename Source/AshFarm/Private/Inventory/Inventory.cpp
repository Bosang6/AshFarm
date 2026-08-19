// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Inventory.h"
#include "Components/BoxComponent.h"

// Sets default values
AInventory::AInventory()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.5f;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if(MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object.Get());
	}
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("碰撞盒"));
	CollisionBox->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionBox->SetBoxExtent(FVector{100.0f, 100.0f, 100.0f});
	CollisionBox->SetRelativeLocation(FVector{0.0f, 0.0f, 20.0f});
	CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

// Called when the game starts or when spawned
void AInventory::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInventory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

