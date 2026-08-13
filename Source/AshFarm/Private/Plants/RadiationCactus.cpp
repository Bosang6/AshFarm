// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/RadiationCactus.h"

URadiationCactus::URadiationCactus()
{
    PlantConfig.PlantName = TEXT("辐射仙人掌");

    // 仙人掌的耗水只有小麦的四分之一 （0.005）
    PlantConfig.WaterConsumption = 0.005f;

    PlantConfig.Sensitivity = 0.5f;
}

// 生长重写
void URadiationCactus::Grow(float DeltaTime, const FPlantGrowthContext& Context)
{
    Super::Grow(DeltaTime, Context);
}

// 评估土壤辐射得到作物生长乘数
float URadiationCactus::EvaluateRadiation(int32 Radiation) const
{  
    if(Radiation == 1)
    {
        return 1.0f;
    }

    return 0.0f;
}

float URadiationCactus::EvaluateSoilType(ESoilType SoilType) const
{
    if(SoilType == ESoilType::Sand)
    {
        return 1.5f;
    }

    return 1.0f;
}