// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/UpgradeSlotComponent.h"
#include "AshFarm.h"
#include "Inventory/Inventory.h"
#include "EngineUtils.h"

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

	// 初始化 InventoryList
	for(TActorIterator<AInventory> It(GetWorld()); It; ++It)
	{
		InventoryList.Add(*It);
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

	// 检查槽位是否已满
	if(InstalledUpgrades.Num() >= MaxSlots)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("槽位已满，无法安装组件!"));
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

	// 检查 Owner 是不是规则里白名单的任意一个 (子类判断)
	for(const auto& OwnerClass : Rule->ValidOwners)
	{
		if(!OwnerClass)
		{
			continue;
		}

		// IsA: 判断 Owner 是不是 OwnerClass 的实例
		if(!Owner->IsA(OwnerClass))
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("%s 不可以安装在 %s 上"), *UpgradeClass->GetName(), *OwnerClass->GetName());
			return false;
		}
	}

	// 检查重复安装
	if(!Rule->bAllowDuplicate)
	{
		// 检查是否已安装相同组件
		for(const auto& InstalledUpgrade : InstalledUpgrades)
		{
			if(IsValid(InstalledUpgrade) && InstalledUpgrade->IsA(UpgradeClass))
			{
				UE_LOG(A_LogAshFarm, Warning, TEXT("%s 已安装在 %s 上"), *UpgradeClass->GetName(), *Owner->GetName());
				return false;
			}
		}
	}

	// 检查兼容性
	if(!CheckCompatibility(UpgradeClass, *Rule))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s 与 %s 不兼容"), *UpgradeClass->GetName(), *Owner->GetName());
		return false;
	}

	// 检查物品资源是否足够
	if(!CheckCost(*Rule))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s 物品资源不足"), *UpgradeClass->GetName());
		return false;
	}

	UE_LOG(A_LogAshFarm, Warning, TEXT("%s 可以安装在 %s 上"), *UpgradeClass->GetName(), *Owner->GetName());
	return true;
}

// 检查组件安装兼容性
bool UUpgradeSlotComponent::CheckCompatibility(TSubclassOf<UActorComponent> UpgradeClass, const FInstallRule& Rule) const
{
	if(!IsValid(UpgradeClass))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("请先设置组件类"));
		return false;
	}

	// 新组件 排斥 已安装的组件
	for(const auto& InstalledUpgrade : InstalledUpgrades)
	{
		if(!IsValid(InstalledUpgrade))
		{
			continue;
		}

		if(Rule.IncompatibleWith.Contains(InstalledUpgrade->GetClass()))
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("%s 与 %s 不兼容"), *UpgradeClass->GetName(), *InstalledUpgrade->GetName());
			return false;
		}
	}

	// 已安装的组件 排斥 新组件
	for(const auto& InstalledUpgrade : InstalledUpgrades)
	{
		if(!IsValid(InstalledUpgrade))
		{
			continue;
		}

		// 获取已安装的组件规则
		const FInstallRule* InstalledUpgradeRule = FindInstallRule(InstalledUpgrade->GetClass());
		// 判断已安装的组件是否排斥新组件
		if(InstalledUpgradeRule->IncompatibleWith.Contains(UpgradeClass))
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("%s 与 %s 不兼容"), *UpgradeClass->GetName(), *InstalledUpgrade->GetName());
			return false;
		}
	}

	return true;
}

// 检查物品资源是否足够
bool UUpgradeSlotComponent::CheckCost(const FInstallRule& Rule) const
{
	if(InventoryList.IsEmpty())
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("请先设置仓库！"));
		return false;
	}

	for(const auto& [Type, Count] : Rule.InstallCost)
	{
		// 检查该仓库物品物资是否足够
		for(const auto& Inventory : InventoryList)
		{
			if(Inventory && Inventory->HasEnoughResources(Type, Count))
			{
				UE_LOG(A_LogAshFarm, Warning, TEXT("%s 缺少物品 %s %d 个"), *Rule.ComponentClass->GetName(), *UEnum::GetValueAsString(Type), Count);
				return false;
			}
		}
	}

	return true;
}

// 安装组件
bool UUpgradeSlotComponent::InstallUpgrade(TSubclassOf<UActorComponent> UpgradeClass)
{
	if(!IsValid(UpgradeClass))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("请先设置组件类！"));
		return false;
	}

	if(!CanInstall(UpgradeClass))
	{
		return false;
	}

	if(!IsValid(Owner))
	{
		return false;
	}

	if(TObjectPtr<UActorComponent> NewUpgrade = Owner->AddComponentByClass(UpgradeClass, false, FTransform::Identity, false))
	{
		InstalledUpgrades.Add(NewUpgrade);
		return true;
	}
	else
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("无法安装 %s"), *UpgradeClass->GetName());
		return false;
	}
}

// 卸载组件
bool UUpgradeSlotComponent::UninstallUpgrade(UActorComponent* Upgrade)
{
	// 检查Owner
	if(!IsValid(Owner))
	{
		return false;
	}

	// 检查要卸载的组件类是否有效
	if(!IsValid(Upgrade))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("请先设置组件类！"));
		return false;
	}

	int32 Index = InstalledUpgrades.Find(Upgrade);
	if(Index == INDEX_NONE)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未安装该组件！"));
		return false;
	}

	InstalledUpgrades.RemoveAt(Index);

	// 如果组件是场景组件，需要先分离组件再销毁组件
	if(TObjectPtr<USceneComponent> SceneComp = Cast<USceneComponent>(Upgrade))
	{
		// 分离场景组件
		SceneComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	Upgrade->UnregisterComponent();
	Upgrade->DestroyComponent();

	return true;
}
