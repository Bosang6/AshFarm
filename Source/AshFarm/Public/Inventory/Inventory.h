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

	// 资源配置数组
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "资源配置表"))
	TArray<FResourcesConfig> ResourcesConfigs;

	// 资源仓库 （资源数量映射）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "资源仓库"))
	TMap<EResourcesType, int32> ResourcesInventory;

	// 已解锁的种子类型, TSubclassOf 是一个类，不是一个实例对象
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "已解锁种子类型"))
	TSet<TSubclassOf<UPlantBase>> UnlockedPlantTypes;


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
