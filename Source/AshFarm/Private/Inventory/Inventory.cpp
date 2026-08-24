// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/Inventory.h"
#include "AshFarm.h"
#include "DrawDebugHelpers.h"

// Sets default values
AInventory::AInventory()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.5f;
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
		case EResourcesType::Fertility :
			return TEXT("肥料");
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

	// 如果接受列表为空，则接受任意资源
	// 检查资源类型是否在可接受列表中
	if(!AcceptedTypes.Contains(Type) && !AcceptedTypes.IsEmpty())
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("AddResource: 资源类型 %s 不在可接受列表中"), *GetResourceTypeText(Type));
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
int32 AInventory::RemoveResources(EResourcesType Type, int32 Count)
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

// 获取资源最大容量
int32 AInventory::GetResourceCapacity(EResourcesType Type) const
{
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

	return Config->MaxCapacity;
}

// 获取资源占比
float AInventory::GetResourcePercentage(EResourcesType Type) const
{
	int Total = 0;

	for(const auto& [Key, Count] : ResourcesInventory)
	{
		Total += Count;
	}

	if(Total <= 0)
	{
		return 0.0f;
	}

	return static_cast<float>(GetResourceCount(Type)) / static_cast<float>(Total);
}

// 获取低库存资源, 例如：占比上限小于20%的资源
TArray<EResourcesType> AInventory::GetLowStockResources(float ThresholdPercent) const
{
	TArray<EResourcesType> LowStockResources;

	if(ThresholdPercent <= 0.0f || ThresholdPercent > 1.0f)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("GetLowStockResources: 占比阈值必须在0到1之间"));
		return LowStockResources;
	}

	for(const auto& Config : ResourcesConfigs)
	{
		int32 CurrentCount = GetResourceCount(Config.Type);
		int32 MaxCapacity = Config.MaxCapacity;

		if(MaxCapacity <= 0)
		{
			continue;
		}

		float CurrentRatio = static_cast<float>(CurrentCount) / static_cast<float>(MaxCapacity);

		if(CurrentRatio < ThresholdPercent)
		{
			LowStockResources.Add(Config.Type);
			UE_LOG(A_LogAshFarm, Warning, TEXT("资源类型 %d 占比为 %.2f, 低于阈值 %.2f"), Config.Type, CurrentRatio, ThresholdPercent);
		}
	}

	return LowStockResources;
}

// ============== 
// 接口实现 
// ==============

// 选中时
void AInventory::OnSelected_Implementation()
{
	Super::OnSelected_Implementation();
	PrintResourceInventory();
}

// 交互时
void AInventory::OnInteract_Implementation()
{
	Super::OnInteract_Implementation();
	// TODO 打开UI面板
}