// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/AutoWateringComponent.h"
#include "Inventory/WaterTank.h"
#include "Actors/PlantBed.h"

// Sets default values for this component's properties
UAutoWateringComponent::UAutoWateringComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;

	// ...
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("自动浇水网格体"));
	Mesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	
	// 加载默认静态网格体 （临时先用一下，后期改）
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/PolygonFarm/Meshes/Props/SM_Prop_LetterBox_01.SM_Prop_LetterBox_01"));
	if(!MeshAsset.Succeeded())
	{
		MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	Mesh->SetStaticMesh(MeshAsset.Object.Get());
	Mesh->SetRelativeLocation(FVector(50.0f, 50.0f, 0.0f));
}


// Called when the game starts
void UAutoWateringComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAutoWateringComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

