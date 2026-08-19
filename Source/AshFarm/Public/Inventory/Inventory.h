// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Inventory/InventoryTypes.h"
#include "Plants/PlantBase.h"

#include "Inventory.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class ASHFARM_API AInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	AInventory();

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
	int32 RemoveResource(EResourcesType Type, int32 Count);

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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Root")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (DisplayName = "植物床网格体"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 碰撞盒组件
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UBoxComponent> CollisionBox;

public:	
	virtual void Tick(float DeltaTime) override;

};
