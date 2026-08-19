// Fill out your copyright notice in the Description page of Project Settings.

#include "AshFarm.h"
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

int32 AInventory::AddResource(EResourcesType Type, int32 Count)
{
	if(Count <= 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource: 添加资源数量必须大于0"));
		return 0;
	}

	// 限制添加资源数量, 不能超过最大值
	// 查询TArray

	// FindByPredicate : 查找数组中的第一个满足要求的元素
	// Lambda表达式: 匿名函数，可以作为一个参数传递
	// [Type]: 捕获：使用函数以外的变量需要进行捕获
	// (const FResourcesConfig& Config): ResourceConfigs里的单个元素
	const FResourcesConfig* Config = ResourcesConfigs.FindByPredicate(
		[Type](const FResourcesConfig& Config)
		{
			return Config.Type == Type;
		}
	);

	// Map查表, 返回value的指针
	const int32* CurrentCount = ResourcesInventory.Find(Type);
	int32 Current = CurrentCount ? *CurrentCount : 0;

	// 还能加入多少资源
	int32 CanAdd = FMath::Min(Count, Config->MaxCapacity - Current);

	if(CanAdd <= 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource: 资源数量超过最大容量"));
		return 0;
	}

	// 存在：覆盖 | 不存在：添加
	ResourcesInventory.Add(Type, Current + CanAdd);

	UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource: 资源添加成功"));
	return CanAdd;
}

// 移除资源
int32 AInventory::RemoveResource(EResourcesType Type, int32 Count)
{
	// TODO
	return 0;
}

// 查询资源数量
int32 AInventory::GetResourceCount(EResourcesType Type)
{
	// TODO
	return 0;
}

// 检查资源是否足够
bool AInventory::HasEnoughResources(EResourcesType Type, int32 Count) const
{	
	// TODO
	return false;
}