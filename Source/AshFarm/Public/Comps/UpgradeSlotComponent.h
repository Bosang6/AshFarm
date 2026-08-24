// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Comps/InstallRule.h"
#include "UpgradeSlotComponent.generated.h"

class AInventory;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UUpgradeSlotComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUpgradeSlotComponent();

	// =====================
	// 配置属性
	// =====================

	// =========== 组件安装规则 =====================
	// 组件安装规则表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "功能组件", meta = (DisplayName = "组件安装规则表"))
	TObjectPtr<UDataTable> InstallRuleTable;
	
	// 最大槽数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "功能组件", meta = (DisplayName = "最大槽数"))
	int32 MaxSlots = 2;

	// =====================
	// 运行时属性
	// =====================

	// 已安装的组件列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "功能组件", meta = (DisplayName = "已安装的组件列表"))
	TArray<TObjectPtr<UActorComponent>> InstalledUpgrades;

	// =====================
	// 功能函数
	// =====================

	// 检查是否可安装组件
	UFUNCTION(BlueprintCallable, Category = "功能组件", meta = (DisplayName = "是否可安装组件"))
	bool CanInstall(TSubclassOf<UActorComponent> UpgradeClass);

	// // 安装组件
	// UPROPERTY(BlueprintCallable, Category = "功能组件", meta = (DisplayName = "安装组件"))
	// bool InstallUpgrade(TSubclassOf<UActorComponent> UpgradeClass);

	// // 卸载组件
	// UPROPERTY(BlueprintCallable, Category = "功能组件", meta = (DisplayName = "卸载组件"))
	// bool InstallUpgrade(TSubclassOf<UActorComponent> UpgradeClass);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// =====================
	// 辅助函数或变量
	// =====================

	TObjectPtr<AActor> Owner;

	TArray<TObjectPtr<AInventory>> InventoryList;

	// 组件安装规则缓存 (静态不能使用UPROPERTY)
	static TMap<TSubclassOf<UActorComponent>, FInstallRule> InstallRuleCache;

	// 查找安装规则
	const FInstallRule* FindInstallRule(TSubclassOf<UActorComponent> UpgradeClass) const;
	
	// 检查组件安装兼容性
	bool CheckCompatibility(TSubclassOf<UActorComponent> UpgradeClass, const FInstallRule& Rule) const;

	// 检查物品资源是否足够
	bool CheckCost(const FInstallRule& Rule) const;
};
