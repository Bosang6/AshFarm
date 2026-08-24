// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Inventory/InventoryTypes.h"
#include "Plants/PlantBase.h"
#include "Interfaces/Interactable.h"
#include "Actors/Building.h"

#include "Inventory.generated.h"

class USceneComponent;
class UStaticMeshComponent;

// 委托: 资源数量低于临界值时触发
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnResourceLow, 
	EResourcesType, ResourcesType,
	int32, CurrentCount,
	int32, MaxCapacity
);

// 委托: 当资源数量高于临界值时，恢复
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnResourceRestore, 
	EResourcesType, ResourcesType
);

UCLASS()
class ASHFARM_API AInventory : public ABuilding
{
	GENERATED_BODY()
	
public:	
	AInventory();

	// 可接受资源类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "可接受资源类型"))
	TSet<EResourcesType> AcceptedTypes;

	// =================
	// 资源报警委托
	// =================

	UPROPERTY(BlueprintAssignable, Category = "仓库", meta = (DisplayName = "当资源数量低于警戒值时"))
	FOnResourceLow OnResourceLow;

	UPROPERTY(BlueprintAssignable, Category = "仓库", meta = (DisplayName = "当资源数量恢复时"))
	FOnResourceRestore OnResourceRestore;


	// =================
	// 仓库数据
	// =================

	// 资源配置数组
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "资源配置表"))
	TArray<FResourcesConfig> ResourcesConfigs;

	// 资源仓库 （资源数量映射）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "资源仓库"))
	TMap<EResourcesType, int32> ResourcesInventory;

	// 已解锁的种子类型, TSubclassOf 是一个类，不是一个实例对象
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "已解锁种子类型"))
	TSet<TSubclassOf<UPlantBase>> UnlockedPlantTypes;


	// =================
	// 仓库功能
	// =================

	// 添加资源
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "添加资源"))
	int32 AddResource(EResourcesType Type, int32 Count);

	// 移除资源
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "移除资源"))
	int32 RemoveResources(EResourcesType Type, int32 Count);

	// 查询资源数量
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "查询资源数量"))
	int32 GetResourceCount(EResourcesType Type) const;

	// 检查资源是否足够
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "检查资源是否足够"))
	bool HasEnoughResources(EResourcesType Type, int32 Count) const;

	// 获取资源类型文本
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "获取资源类型文本"))
	FString GetResourceTypeText(EResourcesType Type) const;

	// 打印资源库存
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "打印资源库存"))
	void PrintResourceInventory() const;

	// 批量添加资源
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "批量添加资源"))
	void AddMultipleResources(const TArray<FResourceBundle>& Bundles);

	// 获取资源最大容量
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "获取资源最大容量"))
	int32 GetResourceCapacity(EResourcesType Type) const;

	// 获取资源占比，例如：水占仓库总库存的百分之多少
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "获取资源占比"))
	float GetResourcePercentage(EResourcesType Type) const;

	// 获取低库存资源, 例如：占比上限小于20%的资源
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "获取低库存资源"))
	TArray<EResourcesType> GetLowStockResources(float ThresholdPercent = 0.2f) const;


	// =================
	// 解锁种子相关
	// =================

	// 解锁种子类型
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "解锁种子类型"))
	void UnlockPlantType(TSubclassOf<UPlantBase> PlantType);

	// 获取已解锁的种子类型
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "获取已解锁的种子类型"))
	TSet<TSubclassOf<UPlantBase>> GetUnlockPlantTypes();

	// 检查种子类型是否已经解锁
	UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "检查种子类型是否已经解锁"))
	bool IsPlantTypeUnlocked(TSubclassOf<UPlantBase> PlantType) const;

	// ========== 接口实现 ==============
	virtual void OnSelected_Implementation() override;
	virtual void OnInteract_Implementation() override;

	// =================
	// 数据统计 排序
	// =================

	// 排序类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "排序类型"))
	EInventorySortType SortType = EInventorySortType::TypeAscending;

	// 排序
	//UFUNCTION(BlueprintCallable, Category = "仓库", meta = (DisplayName = "排序"))
	// TPair无法公开到蓝图
	TArray<TPair<EResourcesType, int32>> GetSortedInventory() const;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
