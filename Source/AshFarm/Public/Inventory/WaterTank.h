// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Inventory.h"
#include "WaterTank.generated.h"

/**
 * 
 */
UCLASS()
class ASHFARM_API AWaterTank : public AInventory
{
	GENERATED_BODY()
	
public:
	// 每次浇水量
	UPROPERTY(EditAnywhere, Category = "土壤", meta = (DisplayName = "每次浇水量"))
	float MoistureAmount = 10.0f;

	// ========== 接口实现 ==============
	virtual bool CanInteractWith_Implementation(const AActor* Target) const override;
	virtual void OnInteractWith_Implementation(AActor* Target) override;
	virtual FString GetAsSelectedHint_Implementation() const override;
};
