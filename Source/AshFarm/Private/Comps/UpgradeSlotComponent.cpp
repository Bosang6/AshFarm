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

// 查找安装规则
const FInstallRule* UUpgradeSlotComponent::FindInstallRule(TSubclassOf<UActorComponent> UpgradeClass) const
{
	if(!InstallRuleTable)
	{
		UE_LOG(A_LogAshFarm, Error, TEXT("请先设置安装规则表!"));
		return nullptr; 
	}

	if(!UpgradeClass)
	{
		UE_LOG(A_LogAshFarm, Error, TEXT("请先设置组件类!"));
		return nullptr;
	}

	// 从缓存中查找
	if(const FInstallRule* Rule = InstallRuleCache.Find(UpgradeClass))
	{
		return Rule;
	}

	// 组件可能是蓝图的组件，比如继承自 UGreenHouseComponent 的子类
	// 因此需要判断 CompClass是否属于规则内的一个子类
	for(const auto& Pair : InstallRuleCache)
	{
		if(UpgradeClass->IsChildOf(Pair.Key))
		{
			return &Pair.Value;
		}
	}

	return nullptr;
}


// 检查是否可安装组件
bool UUpgradeSlotComponent::CanInstall(TSubclassOf<UActorComponent> UpgradeClass)
{
	// 检查Owner
	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Error, TEXT("[UpgradeSlot组件] Owner 无效"));
		return false;
	}

	// 检查安装的组件类是否有效
	if(!UpgradeClass)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("请先设置组件类!"));
		return false;
	}

	// 检查安装规则表是否加载成功
	if(!IsValid(InstallRuleTable))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("请先设置安装规则表!"));
		return false;
	}

	const FInstallRule* Rule = FindInstallRule(UpgradeClass);
	if(!Rule)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s 未找到安装规则!"), *UpgradeClass->GetName());
		return false;
	}

	// 规则白名单中没有指定规则，则不限制安装
	if(Rule->ValidOwners.IsEmpty())
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s 未指定有效安装规则，默认允许安装在所有Actor上"), *UpgradeClass->GetName());
		return true;
	}

	// 子类判断
	for(const auto& OwnerClass : Rule->ValidOwners)
	{
		if(!OwnerClass)
		{
			continue;
		}

		// IsA: 判断 Owner 是不是 OwnerClass 的实例
		if(Owner->IsA(OwnerClass))
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("%s 可以安装在 %s 上"), *UpgradeClass->GetName(), *OwnerClass->GetName());
			return true;
		}
	}

	UE_LOG(A_LogAshFarm, Warning, TEXT("%s 不能安装在 %s 上"), *UpgradeClass->GetName(), *Owner->GetName());
	return false;
}

