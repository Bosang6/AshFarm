// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/GreenHouseComponent.h"

// Sets default values for this component's properties
UGreenHouseComponent::UGreenHouseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlantMesh"));
	Mesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	// ...
}


// Called when the game starts
void UGreenHouseComponent::BeginPlay() 
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGreenHouseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

