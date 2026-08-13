// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/AshPotato.h"
#include "AshFarm.h"

UAshPotato::UAshPotato()
{
    PlantConfig.PlantName = TEXT("灰烬土豆");
    PlantConfig.GrowthSpeed = 0.8f;
    PlantConfig.WaterConsumption = 0.025f;
    PlantConfig.FertilityConsumption = 0.02f;
    PlantConfig.MatureProgress = 80.0f;
    PlantConfig.Sensitivity = 1.5f;
}

// 生长重写
void UAshPotato::Grow(float DeltaTime, const FPlantGrowthContext& Context)
{
    Super::Grow(DeltaTime, Context);
}

// 成熟时调用
void UAshPotato::OnMature()
{
    Super::OnMature();
    UE_LOG(A_LogAshFarm, Warning, TEXT("土豆鼓起来一个包"));
}

// 评估土壤肥力得到作物生长乘数
float UAshPotato::EvaluateFertility(float SoilFertility) const
{
    // 肥力 < 20 停止生长
    if(SoilFertility < 20.0f)
    {
        return 0.0f;
    }
    // 肥力 < 50 正常生长 x0.8
    if(SoilFertility < 50.0f)
    {
        return 0.8f;
    }

    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UAshPotato::EvaluateMoisture(float Moisture) const
{
    // 湿度 > 0.8 正常生长 x0.5
    if(Moisture > 0.8f)
    {
        return 0.5f;
    }

    return 1.0f;
}