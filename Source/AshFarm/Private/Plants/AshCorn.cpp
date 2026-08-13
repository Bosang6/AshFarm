// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/AshCorn.h"
#include "AshFarm.h"

UAshCorn::UAshCorn()
{
    PlantConfig.PlantName = TEXT("灰烬玉米");
    PlantConfig.GrowthSpeed = 2.0f;
    PlantConfig.WaterConsumption = 0.02f;
    PlantConfig.FertilityConsumption = 0.02f;
    PlantConfig.MatureProgress = 80.0f;
}

void UAshCorn::Grow(float DeltaTime, const FPlantGrowthContext& Context)
{


    Super::Grow(DeltaTime, Context);


}

void UAshCorn::OnMature()
{
    Super::OnMature();
    UE_LOG(A_LogAshFarm, Warning, TEXT("%s 可以收割了"), *GetPlantName());
}

// 评估土壤状态得到作物生长乘数
float UAshCorn::EvaluateSoilQuality(ESoilQuality SoilQuality) const
{
    return 1.0f;
}

// 评估土壤肥力得到作物生长乘数
float UAshCorn::EvaluateFertility(float SoilFertility) const
{
    // 肥力降到 30, 速度打 8 折而不是停止
    if(SoilFertility <= 30.0f)
    {
        return 0.8f;
    }

    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UAshCorn::EvaluateMoisture(float Moisture) const
{
    // 湿度 > 0.9, 玉米速度减半（怕涝生效）
    if(Moisture > 0.9f)
    {
        return 0.5f;
    }

    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UAshCorn::EvaluateTemperature(float Temperature) const
{
    // 温度降到 5 以下, 停止生长
    if(Temperature < 5.0f)
    {
        return 0.0f;
    }
    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UAshCorn::EvaluateRadiation(int32 Radiation) const
{
    // 辐射 > 0, 玉米停止生长
    if(Radiation > 0)
    {
        return 0.0f;
    }

    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UAshCorn::EvaluateToxicity(float Toxicity) const
{
    return 1.0f;
}



