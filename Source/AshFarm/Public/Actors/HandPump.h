// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"
#include "Building.h"

#include "HandPump.generated.h"

class AInventory;
class UCoolingComponent;

namespace HandPumpDefaults
{
	// constexpr 编译时计算，不能再运行时修改
	// 不能使用 UPROPERTY，只能用C++的基础类型
	static constexpr float MIN_WATER_FOR_REPAIR 				 = 10.0f;
	static constexpr float REPAIR_RESTORE_PERCENT                = 0.8f;
	static constexpr int32 DEFAULT_REPAIR_ATTEMPTS               = 20;
	static constexpr int32 DEFAULT_MAX_DRY_PUMP_COUNT            = 3;
	static constexpr float DEFAULT_WATER 						 = 40.0f;
	static constexpr float DEFAULT_MAX_WATER 					 = 100.0f;
	static constexpr float DEFAULT_LOW_WATER_THRESHOLD 			 = 0.5f;
	static constexpr float DEFAULT_ADD_WATER_PER_TIME 			 = 10.0f;
	static constexpr float DEFAULT_DURABILITY 					 = 100.0f;
	static constexpr float DEFAULT_MAX_DURABILITY 				 = 100.0f;
	static constexpr float DEFAULT_DURABILITY_LOSS_PER_PUMP 	 = 2.0f;
	static constexpr float DEFAULT_DURABILITY_CRITICAL_THRESHOLD = 10.0f;
	
	
}

UCLASS()
class ASHFARM_API AHandPump : public ABuilding
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandPump();

	// 所属仓库
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "仓库", meta = (DisplayName = "所属仓库"))
	TObjectPtr<AInventory> Inventory = nullptr;

	// ======================
	// 冷却系统
	// ======================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "冷却系统", meta = (DisplayName = "冷却组件"))
	TObjectPtr<UCoolingComponent> CoolingComponent;

	// ======================
	// 水位参数
	// ======================
	#pragma region 水位参数

	// 当前水位
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "水箱状态", meta = (DisplayName = "当前水位", ClampMin = "0"))
	float CurrentWater = HandPumpDefaults::DEFAULT_WATER;

	// 最大水位
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "水箱状态", meta = (DisplayName = "最大水位", ClampMin = "50", ClampMax = "500"))
	float MaxWater = HandPumpDefaults::DEFAULT_MAX_WATER;

	// 每次按下泵增加的水位
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "水箱状态", meta = (DisplayName = "每次按下泵增加的水位"))
	float AddWaterPerTime = HandPumpDefaults::DEFAULT_ADD_WATER_PER_TIME;

	#pragma endregion

	// ======================
	// 手压井状态
	// ======================
	#pragma region 手压井状态

	// 手压井修复次数限制
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "手压井状态", meta = (DispalyName = "手压井修复次数限制", ClampMin = "0"));
	int32 RepairAttempts = HandPumpDefaults::DEFAULT_REPAIR_ATTEMPTS;


	//------------------
	// 耐久度相关
	// -----------------

	// 当耐久度低于阈值时，手压井更容易损坏
	// 当耐久度<=0时，手压井损坏

	// 耐久度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "耐久度"))
	float Durability = HandPumpDefaults::DEFAULT_DURABILITY;
	// 最大耐久度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "最大耐久度", ClampMin = "50.0", ClampMax = "200.0"))
	float MaxDurability = HandPumpDefaults::DEFAULT_MAX_DURABILITY;
	// 泵水损耗
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "泵水损耗", ClampMin = "0.5", ClampMax = "10.0"))
	float DurabilityLossPerPump = HandPumpDefaults::DEFAULT_DURABILITY_LOSS_PER_PUMP;
	// 耐久度危险阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "耐久度危险阈值"))
	float DurabilityCriticalThreshold = HandPumpDefaults::DEFAULT_DURABILITY_CRITICAL_THRESHOLD;
	// -----------------

	// 手压井是否损坏
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "是否损坏"))
	bool bIsBroken = false; // true: 坏

	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "手压井是否损坏"))
	bool IsBroken() const;

	// 手压井是否正在泵水
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "是否正在泵水"))
	bool bIsPumping = false; // true: 正在泵水

	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "手压井是否正在泵水"))
	bool IsPumping() const;

	#pragma endregion
	// ======================
	// 设备名称
	// ======================
	#pragma region 设备名称
	// 设备名称
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "设备名称"))
	FString DeviceName = TEXT("手压井");

	// 设备ID (哈希标识，内部存的是整数ID，大小写不敏感，用于资源路径，标签ID，需要频繁比较的字符串)
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "设备ID"))
	FName DeviceID = FName{"Pump_Default"};

	// 设备描述
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "设备描述"))
	FText DeviceDescription = FText::FromString(TEXT("一口老旧但还能用的手压井"));
	#pragma endregion
	// ======================
	// 功能函数
	// ======================

	// 取水 TakeWater()
	UFUNCTION(BlueprintCallable, Category = "手压井功能", meta = (DisplayName = "取水"))
	float TakeWater(float WaterAmount = 10.0f);

	// 泵水 PumpWater()
	UFUNCTION(BlueprintCallable, Category = "手压井功能", meta = (DisplayName = "泵水"))
	float PumpWater();

	// 修理
	UFUNCTION(BlueprintCallable, Category = "手压井功能", meta = (DisplayName = "修复"))
	bool Repair();

	// 困难: 一键巡检 Maintain()
	UFUNCTION(BlueprintCallable, Category = "NPC能力", meta = (DisplayName = "一键巡检"))
	FString Maintain();

	UFUNCTION(CallInEditor, Category = "手压井功能", meta = (DisplayName = "测试修复"))
	void TestRepair(){ Repair(); }

	// ========== 接口实现 ==============
	virtual void OnSelected_Implementation() override;
	virtual void OnInteract_Implementation() override;
	virtual bool IsInteractable_Implementation() const override;
	virtual void OnUnselected_Implementation() override;

	// ======================
	// 统计和数据函数
	// ======================

	// 手压井泵水次数
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "手压井状态", meta = (DispalyName = "泵水次数", ClampMin = "0"));
	int32 PumpCount = 0;

	// 空转保护
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "手压井状态", meta = (DispalyName = "空转次数", ClampMin = "0"));
	int32 DryPumpCount = 0;

	//获取当前水位占比
	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "获取当前水位占比"))
	float GetWaterPercentage() const;
	//获取当前耐久度占比
	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "获取当前耐久度占比"))
	float GetDurabilityPercentage() const;

	// 打印状态
	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "打印状态"))
	void PrintState();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
