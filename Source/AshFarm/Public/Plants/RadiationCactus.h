// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Plants/PlantBase.h"
#include "RadiationCactus.generated.h"

/**
 * 
 */
UCLASS()
class ASHFARM_API URadiationCactus : public UPlantBase
{
	GENERATED_BODY()
public:

	URadiationCactus();
	
	// 生长重写
	virtual void Grow(float DeltaTime, const FPlantGrowthContext& Context) override;

	// 评估土壤辐射得到作物生长乘数
	virtual float EvaluateRadiation(int32 Radiation) const override;

	virtual float EvaluateSoilType(ESoilType SoilType) const override;
};
