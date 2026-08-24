// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Building.h"
#include "Components/BoxComponent.h"
#include "Comps/HighlightComponent.h"
#include "Comps/UpgradeSlotComponent.h"

#include "AshFarm.h"

// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("高亮组件"));

	DurabilityComponent = CreateDefaultSubobject<UDurabilityComponent>(TEXT("耐久度组件"));

	FloatingTextComponent = CreateDefaultSubobject<UFloatingTextComponent>(TEXT("漂浮文本组件"));

	FeedbackComponent = CreateDefaultSubobject<UFeedbackComponent>(TEXT("音效反馈组件"));
	
	StateTagComponent = CreateDefaultSubobject<UStateTagComponent>(TEXT("状态标签组件"));

	UpgradeSlotComponent = CreateDefaultSubobject<UUpgradeSlotComponent>(TEXT("升级槽组件"));
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	// 绑定委托
	DurabilityComponent->OnCritical.AddDynamic(this, &ABuilding::OnCriticalDurability);
	DurabilityComponent->OnBroken.AddDynamic(this, &ABuilding::OnBroken);

	
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 选中时逻辑
void ABuilding::OnSelected_Implementation()
{
	HighlightComponent->SetHighlight(true);
}

// 取消选中时逻辑
void ABuilding::OnUnselected_Implementation()
{
	HighlightComponent->SetHighlight(false);
}


// 交互时逻辑
void ABuilding::OnInteract_Implementation()
{

}

// 是否可交互
bool ABuilding::IsInteractable_Implementation() const
{
	return !DurabilityComponent->IsBroken();
}

// =================
// 回调函数 
// =================

// 当即将损坏时
void ABuilding::OnCriticalDurability(float Durability)
{
	// TODO: UI 提示设备即将损坏
	FloatingTextComponent->ShowText(TEXT("设备即将损坏"), FColor::Red, 1.0f, 3.0f, 200.0f);
}

// 当损坏时
void ABuilding::OnBroken(float Durability)
{
	// TODO: UI 提示设备损坏

	FloatingTextComponent->ShowText(TEXT("设备损坏"), FColor::Red, 1.0f, 3.0f, 200.0f);
}