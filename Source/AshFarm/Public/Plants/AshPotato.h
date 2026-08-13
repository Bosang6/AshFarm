// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Plants/PlantBase.h"
#include "Actors/PlantBedTypes.h"

#include "AshPotato.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "灰烬土豆基类"))
class ASHFARM_API UAshPotato : public UPlantBase
{
	GENERATED_BODY()
	
public:

	UAshPotato();

	// 生长重写
	virtual void Grow(float DeltaTime, const FPlantGrowthContext& Context) override;

	// 成熟时调用
	virtual void OnMature() override;

	// 评估土壤肥力得到作物生长乘数
	virtual float EvaluateFertility(float SoilFertility) const override;

	// 评估土壤水分得到作物生长乘数
	virtual float EvaluateMoisture(float Moisture) const override;


};
