// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/HandPump.h"
#include "AshFarm.h"
#include "Comps/CoolingComponent.h"
#include "Comps/HighlightComponent.h"
#include "Inventory/Inventory.h"

// Sets default values
AHandPump::AHandPump()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 创建组件
	// CreateDefaultSubobject 只能在构造函数中使用
	CoolingComponent = CreateDefaultSubobject<UCoolingComponent>(TEXT("冷却组件"));
	HighlightComponent = CreateDefaultSubobject<UHighlightComponent>(TEXT("高亮组件"));

}

// Called when the game starts or when spawned
void AHandPump::BeginPlay()
{
	Super::BeginPlay();
	
	ensureAlwaysMsgf(MaxWater > 0.0f, TEXT("MaxWater 必须大于 0.0f"));
	ensureAlwaysMsgf(MaxDurability > 0.0f, TEXT("MaxDurability 必须大于 0.0f"));
	ensureAlwaysMsgf(Mesh != nullptr, TEXT("Mesh 不能为空"));
}

// Called every frame
void AHandPump::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

bool AHandPump::IsBroken() const
{
	return bIsBroken;
}

bool AHandPump::IsPumping() const
{
	return bIsPumping;
}

float AHandPump::TakeWater(float WaterAmount)
{
	// 检查以下几种情况
	
	//1. 请求不合理 WaterAmount <= 0.0f
	if(WaterAmount <= 0.0f)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井: %s 取水请求不合理"), *DeviceID.ToString());
		return 0.0f;
	}
	//2. 设备坏了 bIsBroken == true
	if(bIsBroken == true)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井: %s 已损坏"), *DeviceID.ToString());
		return 0.0f;
	}

	float AvailableWater = CurrentWater - HandPumpDefaults::MIN_WATER_FOR_REPAIR;

	// 中等 1
	if(CurrentWater <= HandPumpDefaults::MIN_WATER_FOR_REPAIR)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井: %s 水箱剩余: %f, 再抽水后就无法修复"), *DeviceID.ToString(), CurrentWater);
		return 0.0f;
	}
	//3. 水箱空了 CurrentWater <= 0
	if(AvailableWater <= 0.0f)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井: %s 水箱已到保底水位"), *DeviceID.ToString());
		return 0.0f;
	}
	//4. 水位不足
	float FinalWaterAmount = FMath::Min(AvailableWater, WaterAmount);
	CurrentWater -= FinalWaterAmount;

	// 复位空转次数
	DryPumpCount = 0;

	UE_LOG(A_LogAshFarm, Warning, TEXT("手压井: %s 取水 %f, 剩余: %f"), *DeviceID.ToString(), FinalWaterAmount, CurrentWater);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("手压井: %s 取水 %f, 剩余: %f"), *DeviceID.ToString(), FinalWaterAmount, CurrentWater));
	return FinalWaterAmount;
}

float AHandPump::PumpWater()
{
	if(!IsValid(CoolingComponent))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 没有冷却组件"), *DeviceID.ToString());
		return 0.0f;
	}

	// 中等3
	if(!bIsBroken && CurrentWater < MaxWater)
	{
		DryPumpCount = 0;

		if(!bIsBroken && CurrentWater < MaxWater && !bIsPumping)
		{
			// float LastWater = CurrentWater; 
			// CurrentWater = FMath::Clamp(CurrentWater + AddWaterPerTime, 0.0f, MaxWater);

			if(CoolingComponent->bOverHeat)
			{
				UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 已过热, 无法继续泵水"), *DeviceID.ToString());
				return 0.0f;
			}

			// 检查手压井是否有仓库
			/*
				IsValid:
					1. 判断指针是否为空
					2. 判断对象是否被GC标记
					3. 判断对象是否被损坏
			*/
			if(!IsValid(Inventory))
			{
				UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 所属仓库为空"), *DeviceID.ToString());
				return 0.0f;
			}

			bIsPumping = true;
			// 手压井在使用时禁用自然散热
			CoolingComponent->SetEnabledCooling(false);

			// 往 Inventory 仓库水箱内装水
			int32 AddWater = Inventory->AddResource(EResourcesType::Water, AddWaterPerTime);

			// 增加热量
			CoolingComponent->AddHeat();

			// 耐久度损耗
			Durability = FMath::Clamp(Durability - DurabilityLossPerPump, 0.0f, MaxDurability);
			// 检查手压井是否低于危险阈值
			if(Durability <= DurabilityCriticalThreshold)
			{
				UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 耐久度: %f 低于危险阈值: %f"), *DeviceID.ToString(), Durability, DurabilityCriticalThreshold);
			}

			if(Durability <= 0)
			{
				bIsBroken = true;
				UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 已损坏"), *DeviceID.ToString());
			}

			// 记录泵水次数
			PumpCount++;

			// TCHAR*
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("手压井ID: %s, 当前水位: %f, 泵水次数: %d"), *DeviceID.ToString(), CurrentWater, PumpCount));

			bIsPumping = false;
			// 手压井在使用结束时启用自然散热
			CoolingComponent->SetEnabledCooling(true);

			return AddWater;
		}
	}
	else
	{
		DryPumpCount++;

		if(DryPumpCount >= HandPumpDefaults::DEFAULT_MAX_DRY_PUMP_COUNT)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s, 别再压了, 手柄快断了!"), *DeviceID.ToString());
		}

		FString Reason = TEXT("");

		if(bIsBroken)
		{
			Reason = TEXT("手压井已损坏, 需要修复");
		}
		else if(CurrentWater >= MaxWater)
		{
			Reason = TEXT("水箱已到最大水位, 无法继续泵水");
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("手压井ID: %s 泵水失败: %s"), *DeviceID.ToString(), *Reason));

		return 0.0f;		
	}

	return 0.0f;
}

//获取当前水位占比
float AHandPump::GetWaterPercentage() const
{
	ensure(MaxWater > 0.0f);

	if(MaxWater == 0.0f)
	{
		return 0.0f;
	}

	return CurrentWater / MaxWater;
}

//获取当前耐久度占比
float AHandPump::GetDurabilityPercentage() const
{
	ensure(MaxDurability > 0.0f);

	if(MaxDurability == 0.0f)
	{
		return 0.0f;
	}

	return Durability / MaxDurability;
}

bool AHandPump::Repair()
{
	// 检查是否损坏
	if(!bIsBroken)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 未损坏, 无需修复"), *DeviceID.ToString());
		return false;
	}

	// 检查修复次数
	if(RepairAttempts <= 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 无修复次数"), *DeviceID.ToString());
		return false;
	}

	// 检查水位是否足够维修
	if(CurrentWater < HandPumpDefaults::MIN_WATER_FOR_REPAIR)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 水位不足, 无法修复"), *DeviceID.ToString());
		return false;
	}

	RepairAttempts--;

	// 中等2

	CurrentWater -= HandPumpDefaults::MIN_WATER_FOR_REPAIR;
	CurrentWater = FMath::Clamp(CurrentWater, 0.0f, MaxWater);

	DryPumpCount = 0;

	// TODO: 需要耗材修复

	// 恢复耐久度
	Durability = MaxDurability * HandPumpDefaults::REPAIR_RESTORE_PERCENT;

	bIsBroken = false;

	UE_LOG(A_LogAshFarm, Warning, TEXT("手压井ID: %s 已修复, 修复剩余次数: %d, 耐久度恢复到: %f"), *DeviceID.ToString(), RepairAttempts, Durability);
	
	return true;
}

FString AHandPump::Maintain()
{
	FString Report = TEXT("");  //巡检报告
	bool bDidSomething = false; //是否有没有干活

	// 1.检查手压井是否损坏，并尝试修复
	if(bIsBroken)
	{
		// TODO: Repair只返回是否修复成功, 没有返回失败原因, 待优化
		if(Repair())
		{
			Report += FString::Printf(TEXT("手压井坏了! 已修复"));
			bDidSomething = true;
		}
		else
		{
			Report += FString::Printf(TEXT("手压井彻底损坏! 修复失败"));
			return Report;
		}
	}

	// 2.检查耐久度
	if(Durability <= DurabilityCriticalThreshold)
	{
		Report += FString::Printf(TEXT("手压井耐久度低于危险阈值, 再压几下就要散架了"));
		bDidSomething = true;
	}

	// 3。检查水位量
	if(GetWaterPercentage() <= HandPumpDefaults::DEFAULT_LOW_WATER_THRESHOLD)
	{
		float PumpedWater = PumpWater();
		Report += FString::Printf(TEXT("手压井水位不足, 已泵水 %f 单位水"), PumpedWater);
		bDidSomething = true;
	}

	if(!bDidSomething)
	{
		Report += FString::Printf(TEXT("手压井一切正常"));
	}

	return Report;
}

// 打印状态
void AHandPump::PrintState()
{
	FString PrintString = TEXT("==== 手压井状态 ====");

	PrintString += FString::Printf(
		TEXT("\n当前耐久度: %f, 当前修复次数: %d, 当前泵水次数: %d"),
		GetDurabilityPercentage(),
		RepairAttempts,
		PumpCount
	);

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

// ============== 
// 接口实现 
// ==============

// 选中时
void AHandPump::OnSelected_Implementation()
{
	PrintState();
	HighlightComponent->SetHighlight(true);
}

// 交互时
void AHandPump::OnInteract_Implementation()
{
	PumpWater();
}

bool AHandPump::IsInteractable_Implementation() const
{
	return Durability > 0.0f && !CoolingComponent->bOverHeat;
}

// 取消选中时
void AHandPump::OnUnselected_Implementation()
{
	HighlightComponent->SetHighlight(false);
}

