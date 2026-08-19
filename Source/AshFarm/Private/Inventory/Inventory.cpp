// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/Inventory.h"
#include "AshFarm.h"
#include "DrawDebugHelpers.h"
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

	PrintResourceInventory();
}

// 获取资源类型文本
FString AInventory::GetResourceTypeText(EResourcesType Type) const
{
	switch (Type)
	{
		case EResourcesType::Water :
			return TEXT("水");
		case EResourcesType::Wood :
			return TEXT("木材");
		case EResourcesType::Soil :
			return TEXT("泥土");
		case EResourcesType::Waste :
			return TEXT("废料");
		case EResourcesType::Gold :
			return TEXT("金币");
		case EResourcesType::Tool :
			return TEXT("工具");
		case EResourcesType::Plant :
			return TEXT("种子");
		
		default:
			return TEXT("未知");
	}
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

	if(!Config)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource: 添加资源类型 %s 不存在"), *GetResourceTypeText(Type));
		return 0;
	}

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

	UE_LOG(A_LogAshFarm, Warning, TEXT("仓库资源 %s 剩余数量: %d"), *GetResourceTypeText(Type), Current + CanAdd);

	return CanAdd;
}

// 移除资源
int32 AInventory::RemoveResource(EResourcesType Type, int32 Count)
{
	if(Count <= 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("RemoveResource: 移除资源数量必须大于0"));
		return 0;
	}

	// 查库存
	const int32* CurrentCount = ResourcesInventory.Find(Type);
	if(!CurrentCount || *CurrentCount < 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("RemoveResource: 资源库存为0"));
		return 0;
	}
	
	int32 Current = *CurrentCount;
	int32 CanRemove = FMath::Min(Current, Count);

	// 移除资源
	int32 NewCount = Current - CanRemove;
	if(NewCount > 0)
	{
		ResourcesInventory.Add(Type, NewCount);
	}
	else
	{
		ResourcesInventory.Remove(Type);
	}

	UE_LOG(A_LogAshFarm, Warning, TEXT("仓库资源 %s 剩余数量: %d"), *GetResourceTypeText(Type), NewCount);

	return CanRemove;
}

// 查询资源数量
int32 AInventory::GetResourceCount(EResourcesType Type) const
{
	const int32* CurrentCount = ResourcesInventory.Find(Type);
	return CurrentCount ? *CurrentCount : 0;
}

// 检查资源是否足够
bool AInventory::HasEnoughResources(EResourcesType Type, int32 Count) const
{	
	return GetResourceCount(Type) >= Count; 
} 

// 打印资源库存
void AInventory::PrintResourceInventory() const
{
	FString PrintString = TEXT("==== 仓库资源库存 ====");

	if(ResourcesInventory.IsEmpty())
	{
		PrintString += TEXT("\n\n仓库资源库存为空");
	}
	else
	{
		auto SortedInventory = GetSortedInventory();

		// const TPair<EResourcesType, int32>& Pair : ResourcesInventory
		// const auto& Pair : ResourcesInventory
		for(const TPair<EResourcesType, int32>& Pair : SortedInventory)
		{
			PrintString += FString::Printf(TEXT("\n%s: %d"), *GetResourceTypeText(Pair.Key), Pair.Value);
		}
	}

	FVector TextLocation = GetActorLocation() + FVector(0.0f, 0.0f, 200.0f); // 土壤肥力文本位置
		
	// DEBUG 打印信息
	DrawDebugString(
		GetWorld(), 
		TextLocation, 
		PrintString, 
		nullptr, 
		FColor::White, 
		0.5f	//文本显示时间
	); 

}

void AInventory::AddMultipleResources(const TArray<FResourceBundle>& Bundles)
{
	for(const FResourceBundle& Bundle : Bundles)
	{
		if(Bundle.Count <= 0)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("AddMultipleResources: 添加资源数量必须大于0"));
			continue;
		}

		AddResource(Bundle.Type, Bundle.Count);
	}
}

// 解锁种子类型
void AInventory::UnlockPlantType(TSubclassOf<UPlantBase> PlantType)
{
	if(!PlantType)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("UnlockPlantType: 种子类型为空"));
		return;
	}

	if(UnlockedPlantTypes.Contains(PlantType))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("UnlockPlantType: 种子类型已解锁"));
		return;
	}

	UnlockedPlantTypes.Add(PlantType);
	UE_LOG(A_LogAshFarm, Warning, TEXT("UnlockPlantType: 解锁种子类型: %s"), *PlantType->GetName());
}

// 检查种子类型是否已经解锁
bool AInventory::IsPlantTypeUnlocked(TSubclassOf<UPlantBase> PlantType) const
{
	return UnlockedPlantTypes.Contains(PlantType);
}

// 获取已解锁的种子类型
TSet<TSubclassOf<UPlantBase>> AInventory::GetUnlockPlantTypes()
{
	return UnlockedPlantTypes;
}

// 排序
TArray<TPair<EResourcesType, int32>> AInventory::GetSortedInventory() const
{
	TArray<TPair<EResourcesType, int32>> SortedInventory;
	for(const auto& Pair : ResourcesInventory)
	{
		SortedInventory.Add(Pair);
	}

	switch (SortType)
	{
		case EInventorySortType::CountAscending:
			SortedInventory.Sort(
				[](const TPair<EResourcesType, int32>& A, const TPair<EResourcesType, int32>& B)
				{
					return A.Value < B.Value;
				}
			);
			break;
		case EInventorySortType::CountDescending:
			SortedInventory.Sort(
				[](const TPair<EResourcesType, int32>& A, const TPair<EResourcesType, int32>& B)
				{
					return A.Value > B.Value;
				}
			);
			break;
		case EInventorySortType::TypeAscending:
			SortedInventory.Sort(
				[](const TPair<EResourcesType, int32>& A, const TPair<EResourcesType, int32>& B)
				{
					return static_cast<uint8>(A.Key) < static_cast<uint8>(B.Key);
				}
			);
			break;
		case EInventorySortType::TypeDescending:
			SortedInventory.Sort(
				[](const TPair<EResourcesType, int32>& A, const TPair<EResourcesType, int32>& B)
				{
					return static_cast<uint8>(A.Key) > static_cast<uint8>(B.Key);
				}
			);
			break;
	}

	return SortedInventory;
}
