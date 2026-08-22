// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CoolingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UCoolingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCoolingComponent();

	// 是否启用冷却系统
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "冷却系统", meta = (DisplayName = "是否启用冷却系统"))
	bool bEnabled = true;

	// 设置是否启用组件
	UFUNCTION(BlueprintCallable, Category = "冷却系统", meta = (DisplayName = "设置是否启用"))
	void SetEnabled(bool Enabled) { bEnabled = Enabled; };

	// 是否启用自然散热
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "冷却系统", meta = (DisplayName = "是否启用自然散热"))
	bool bEnabledCooling = true;

	// 设置是否启用自然散热
	UFUNCTION(BlueprintCallable, Category = "冷却系统", meta = (DisplayName = "设置是否启用"))
	void SetEnabledCooling(bool Enabled) { bEnabledCooling = Enabled; };

#pragma region 冷却系统

	// 当前热量
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "冷却系统", meta = (DisplayName = "当前热量"))
	float CurrentHeat = 0.0f; 

	// 最大热量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "冷却系统", meta = (DisplayName = "最大热量", ClampMin = "50"))
	float MaxHeat = 100.0f; 

	// 是否过热
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "冷却系统", meta = (DisplayName = "是否过热"))
	bool bOverHeat = false; 

	// 是否真正冷却中
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "冷却系统", meta = (DisplayName = "是否真正冷却中"))
	bool bIsCooling = false; 

	// 自然散热速率
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "冷却系统", meta = (DisplayName = "自然散热速率", ClampMin = "1"))
	float CooldownRate = 15.0f; 

	// 每次使用增加的热量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "冷却系统", meta = (DisplayName = "每次使用增加的热量",  ClampMin = "1"))
	float HeatPerUse = 15.0f; 

	// 获取当前热量占比
	UFUNCTION(BlueprintCallable, Category = "冷却系统", meta = (DisplayName = "获取当前热量占比"))
	float GetHeatPercentage() const;

	// 增加热量
	UFUNCTION(BlueprintCallable, Category = "冷却系统", meta = (DisplayName = "增加热量"))
	void AddHeat();

	// 检查是否正在冷却中
	UFUNCTION(BlueprintCallable, Category = "冷却系统", meta = (DisplayName = "检查是否正在冷却中"))
	bool IsCooling() const { return bIsCooling; };

#pragma endregion

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 缓存拥有者
	TObjectPtr<AActor> Owner;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
