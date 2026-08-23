// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/UpgradeSlotComponent.h"
#include "AshFarm.h"

TMap<TSubclassOf<UActorComponent>, FInstallRule> UUpgradeSlotComponent::InstallRuleCache = {};

// Sets default values for this component's properties
UUpgradeSlotComponent::UUpgradeSlotComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUpgradeSlotComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	// 初始化 Owner
	Owner = GetOwner();
	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Error, TEXT("[UpgradeSlot组件] Owner 无效"));
	}

	// 加载 组件安装规则表
	if(!InstallRuleTable)
	{
		InstallRuleTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/0_/Comps/DT_InstallRule.DT_InstallRule"));
	}

	if(!IsValid(InstallRuleTable))
	{
		UE_LOG(A_LogAshFarm, Error, TEXT("组件安装规则表加载失败, 请检查 /Game/0_/Comps/DT_InstallRule.DT_InstallRule 是否存在"));
	}
	else
	{
		UE_LOG(A_LogAshFarm, Error, TEXT("组件安装规则表加载成功, 共加载 %d 条规则"), InstallRuleTable->GetRowNames().Num());
	}

	// 将安装规则表中的规则缓存到InstallRuleCache中
	TArray<FName> RowNames = InstallRuleTable->GetRowNames();

	for(const FName& RowName : RowNames)
	{
		const FInstallRule* Rule = InstallRuleTable->FindRow<FInstallRule>(RowName, TEXT(""));

		if(!Rule)
		{
			UE_LOG(A_LogAshFarm, Error, TEXT("组件安装规则表加载失败，请检查 %s 是否存在"), *RowName.ToString());
			continue;
		}

		if(!Rule->ComponentClass)
		{
			UE_LOG(A_LogAshFarm, Error, TEXT("组件安装规则表加载失败，请检查 %s 的 ComponentClass 是否存在"), *RowName.ToString());
			continue;
		}

		InstallRuleCache.Add(Rule->ComponentClass, *Rule);
		UE_LOG(A_LogAshFarm, Error, TEXT("组件安装规则表加载成功， %s"), *RowName.ToString());
	}
	
}


// Called every frame
void UUpgradeSlotComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

