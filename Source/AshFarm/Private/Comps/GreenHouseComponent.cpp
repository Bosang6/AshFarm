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
	
	// 加载默认静态网格体
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/PolygonFarm/Meshes/Buildings/SM_Bld_Greenhouse_01.SM_Bld_Greenhouse_01"));
	if(!MeshAsset.Succeeded())
	{
		MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	Mesh->SetStaticMesh(MeshAsset.Object.Get());
}


// Called when the game starts
void UGreenHouseComponent::BeginPlay() 
{
	Super::BeginPlay();

	// 获取Owner的包围盒
	FVector Origin, BoxExtent;
	GetOwner()->GetActorBounds(false, Origin, BoxExtent);  // 第一个参数：是否考虑碰撞盒的大小

	// 获取当前Mesh的包围盒
	FVector OriginMesh, BoxExtentMesh;
	Mesh->GetLocalBounds(OriginMesh, BoxExtentMesh);

	FVector Scale = BoxExtent / BoxExtentMesh;

	// 设置 Mesh 网格体缩放
	Mesh->SetRelativeScale3D(Scale);
}


// Called every frame
void UGreenHouseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

