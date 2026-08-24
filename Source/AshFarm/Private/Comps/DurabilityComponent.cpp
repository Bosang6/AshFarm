// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/DurabilityComponent.h"
#include "AshFarm.h"

// Sets default values for this component's properties
UDurabilityComponent::UDurabilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDurabilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	if(!IsValid(GetOwner()))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("耐久度组件: Owner 无效"));
		return;
	}
	Owner = GetOwner();	
	
	ensureAlwaysMsgf(MaxDurability > 0.0f, TEXT("MaxDurability 必须大于 0.0f"));
}


// Called every frame
void UDurabilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 是否损坏
bool UDurabilityComponent::IsBroken() const
{
	return bIsBroken;
}

// 是否低于耐久度危险阈值
bool UDurabilityComponent::IsCritical() const
{
	return Durability <= DurabilityCriticalThreshold;
}

//获取当前耐久度占比
float UDurabilityComponent::GetDurabilityPercentage() const
{
	ensure(MaxDurability > 0.0f);

	if(MaxDurability == 0.0f)
	{
		return 0.0f;
	}

	return Durability / MaxDurability;
}

// 修理
bool UDurabilityComponent::Repair()
{
	// 检查是否损坏
	if(!bIsBroken)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s: 未损坏, 无需修复"), *Owner->GetName());
		return false;
	}

	if(!bCanUnlimitRepair)
	{
		// 检查修复次数
		if(RepairAttempts <= 0)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("%s: 无修复次数"), *Owner->GetName());
			return false;
		}

		RepairAttempts--;
	}

	// TODO: 需要耗材修复

	// 恢复耐久度
	Durability = MaxDurability * RepairRestorePercent;

	bIsBroken = false;

	UE_LOG(A_LogAshFarm, Warning, TEXT("%s: 已修复, 修复剩余次数: %d, 耐久度恢复到: %f"), *Owner->GetName(), RepairAttempts, Durability);
	
	return true;
}

// Take Damage 承受伤害
float UDurabilityComponent::TakeDamage(float DamageAmount)
{
	if(!IsValid(Owner)) return 0.0f;

	if(DamageAmount <= 0.0f) return 0.0f;

	if(bIsBroken)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s: 已损坏, 无法减少耐久度"), *Owner->GetName());
		return 0.0f;
	}

	float ActualDamage = FMath::Min(DamageAmount, Durability);

	Durability -= DamageAmount;
	Durability = FMath::Clamp(Durability, 0.0f, MaxDurability);

	if(IsCritical())
	{
		// 广播委托, 耐久度危险
		OnCritical.Broadcast(Durability);
	}

	if(Durability <= 0.0f)
	{
		// 广播委托, 耐久度为0
		OnBroken.Broadcast(Durability);
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s: 已损坏, 耐久度为0"), *Owner->GetName());
	}

	//返回实际伤害量
	return ActualDamage;
}

// 获取可修复次数
int32 UDurabilityComponent::GetRepairAttempts() const
{
	return RepairAttempts;
}