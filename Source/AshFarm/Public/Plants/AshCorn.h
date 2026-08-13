// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plants/PlantBase.h"
#include "Actors/PlantBedTypes.h"

#include "AshCorn.generated.h"

/**
 * 玉米类
 */
UCLASS(BlueprintType, Blueprintable, meta = (DisplayName = "灰烬玉米基类"))
class ASHFARM_API UAshCorn : public UPlantBase
{
	GENERATED_BODY()
public:
	UAshCorn();

	// 生长重写
	virtual void Grow(float DeltaTime, const FPlantGrowthContext& Context) override;

	// 成熟时调用
	virtual void OnMature() override;

	// 评估土壤状态得到作物生长乘数
	virtual float EvaluateSoilQuality(ESoilQuality SoilQuality) const override;

	// 评估土壤肥力得到作物生长乘数
	virtual float EvaluateFertility(float SoilFertility) const override;

	// 评估土壤水分得到作物生长乘数
	virtual float EvaluateMoisture(float Moisture) const override;

	// 评估土壤水分得到作物生长乘数
	virtual float EvaluateTemperature(float Temperature) const override;

	// 评估土壤水分得到作物生长乘数
	virtual float EvaluateRadiation(int32 Radiation) const override;

	// 评估土壤水分得到作物生长乘数
	virtual float EvaluateToxicity(float Toxicity) const override;

	
};
