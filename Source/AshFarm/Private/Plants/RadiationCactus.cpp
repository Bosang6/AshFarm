// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/RadiationCactus.h"

URadiationCactus::URadiationCactus()
{
    RowName = TEXT("辐射仙人掌");
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