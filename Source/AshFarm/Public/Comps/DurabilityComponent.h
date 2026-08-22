// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DurabilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UDurabilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDurabilityComponent();

	// 手压井修复次数限制
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "手压井状态", meta = (DispalyName = "手压井修复次数限制", ClampMin = "0"));
	int32 RepairAttempts = 0;  // 0 表示无限修复

	// 耐久度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "耐久度"))
	float Durability = 100.0f;

	// 最大耐久度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "最大耐久度", ClampMin = "50.0", ClampMax = "200.0"))
	float MaxDurability = 100.0f;
	
	// 耐久度危险阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "耐久度危险阈值"))
	float DurabilityCriticalThreshold = 10.0f;

	// 耐久度修复百分比
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "耐久度修复百分比", ClampMin = "0.0", ClampMax = "1.0"))
	float RepairRestorePercent = 1.0f;

	// 是否损坏
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "手压井状态", meta = (DisplayName = "是否损坏"))
	bool bIsBroken = false; // true: 坏

	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "手压井是否损坏"))
	bool IsBroken() const;

	// 是否低于耐久度危险阈值
	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "是否低于耐久度危险阈值"))
	bool IsCritical() const;

	// 获取可修复次数
	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "获取可修复次数"))
	int32 GetRepairAttempts() const;

	// 修理
	UFUNCTION(BlueprintCallable, Category = "手压井功能", meta = (DisplayName = "修复"))
	bool Repair();

	//获取当前耐久度占比
	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "获取当前耐久度占比"))
	float GetDurabilityPercentage() const;

	// Take Damage 承受伤害
	UFUNCTION(BlueprintCallable, Category = "手压井状态", meta = (DisplayName = "承受耐久度损耗"))
	float TakeDamage(float DamageAmount);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Owner
	TObjectPtr<AActor> Owner = nullptr;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
