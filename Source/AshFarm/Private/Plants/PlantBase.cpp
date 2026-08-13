// Fill out your copyright notice in the Description page of Project Settings.


#include "Plants/PlantBase.h"
#include "AshFarm.h"
#include "Actors/PlantBed.h"

// 初始化植物配置表 变量
TObjectPtr<UDataTable> UPlantBase::PlantDataTable = nullptr;

// 当构造完成后调用
void UPlantBase::PostInitProperties()
{
    Super::PostInitProperties();

    // 加载植物配置数据表
    LoadPlantDataTable();

    // 从植物配置数据表加载植物属性
    LoadFromPlantDataTable();
}

// 编辑器专用回调, 在Detail面板中, 任何数据被修改都会被调用。用于更新编辑时状态，便于策划观察。
void UPlantBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    // NAME_None 为 FName 的空值
    // 当RowName改变时，且不为空，从植物配置表数据表加载数据 
    if(RowName != NAME_None)
    {
        // 加载植物配置数据表
        LoadPlantDataTable();

        // 从植物配置数据表加载植物属性
        LoadFromPlantDataTable();
    }
}

// 加载植物配置数据表
void UPlantBase::LoadPlantDataTable()
{
    UPlantBase::PlantDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/0_/Plants/DT_Plants.DT_Plants"));

    if(!UPlantBase::PlantDataTable)
    {
        UE_LOG(A_LogAshFarm, Error, TEXT("植物配置数据表: 加载失败"));
    }
    else
    {
        UE_LOG(A_LogAshFarm, Log, TEXT("植物配置数据表: 加载成功"));
    }

}

// 通过 RowName行命名 从植物配置表加载植物属性
bool UPlantBase::LoadFromPlantDataTable()
{
    if(!UPlantBase::PlantDataTable)
    {
        UE_LOG(A_LogAshFarm, Error, TEXT("植物配置数据表: 未加载"))
        return false;
    }

    FPlantConfig* FoundRow = UPlantBase::PlantDataTable->FindRow<FPlantConfig>(RowName, TEXT(""));

    if(!FoundRow)
    {
        UE_LOG(A_LogAshFarm, Error, TEXT("植物配置数据表: 未找到行命名未 %s 的行"), *RowName.ToString());
        return false;
    }

    PlantConfig = *FoundRow;
    UE_LOG(A_LogAshFarm, Log, TEXT("植物配置数据表: 加载成功, 植物名称: %s, 生长速度: %f"), *PlantConfig.PlantName, PlantConfig.GrowthSpeed);

    return true;
}

// 生长
void UPlantBase::Grow(float DeltaTime, const FPlantGrowthContext& Context
    )
{
    // 没有水或已成熟就不生长
    if(Context.Moisture <= 0.0f || bIsMature)
    {
        return;
    }

    float EvaluatedMulti = EvaluateSoilQuality(Context.SoilQuality) *
        EvaluateSoilType(Context.SoilType) *
        EvaluateFertility(Context.Fertility) * 
        EvaluateMoisture(Context.Moisture) *
        EvaluateTemperature(Context.Temperature) *
        EvaluateRadiation(Context.RadiationLevel) *
        EvaluateToxicity(Context.Toxicity);

    // 作物逆境值计算
    if(EvaluatedMulti < 1.0f)
    {
        Stress += (1.0f - EvaluatedMulti) * PlantConfig.Sensitivity * DeltaTime;
        Stress = FMath::Clamp(Stress, 0.0f, 100.0f);
        SetPlantQuality();
    }

    // 作物成长进度
    GrowthProgress += PlantConfig.GrowthSpeed * EvaluatedMulti * DeltaTime;

    //Clamp
    GrowthProgress = FMath::Clamp(GrowthProgress, 0.0f, PlantConfig.MatureProgress);

    if(GrowthProgress >= PlantConfig.MatureProgress)
    {
        bIsMature = true;
        OnMature();
    }

    // 生长过后判断作物成长状态
    SetGrowthStage();
}

// 获取当前生长阶段文字
FString UPlantBase::GetGrowthStageText(EGrowthStage InGrowthStage) const
{
    switch(InGrowthStage)
    {
        case EGrowthStage::Seedling:
            return TEXT("幼苗期");
        case EGrowthStage::Growing:
            return TEXT("生长期");
        case EGrowthStage::Flowering:
            return TEXT("开花期");
        case EGrowthStage::Mature:
            return TEXT("成熟期");

        default:
            return TEXT("");
    }
}

// 设置生长状态, 判断的阈值还是用之前一样的
void UPlantBase::SetGrowthStage()
{
    float GrowthProgressRatio = GrowthProgress / PlantConfig.MatureProgress;

    if(GrowthProgressRatio < PlantDefaults::GROWTH_PROGRESS_THRESHOLD)
    {
        GrowthStage = EGrowthStage::Seedling;
        return;
    }
    else if(GrowthProgressRatio < PlantDefaults::FLOWERING_PROGRESS_THRESHOLD)
    {
        GrowthStage = EGrowthStage::Growing;
        return;
    }
    else if(GrowthProgressRatio < 1.0f)
    {
        GrowthStage = EGrowthStage::Flowering;
        return;
    }
    else
    {
        GrowthStage = EGrowthStage::Mature;
        return;
    }
}

// 当成熟时调用
void UPlantBase::OnMature()
{
    SetPlantQuality();
    UE_LOG(A_LogAshFarm, Warning, TEXT("收获了 %s (品质: %s)"), *GetPlantName(), *GetQualityText(CurrentQuality));
}

UStaticMesh* UPlantBase::GetStageMesh() const
{

    EGrowthStage CurrentGrowthStage = GetGrowthStage();

    switch(CurrentGrowthStage)
    {
        case EGrowthStage::Seedling:
            return PlantConfig.SeedlingMesh.LoadSynchronous();
        case EGrowthStage::Growing:
            return PlantConfig.GrowthMesh.LoadSynchronous();
        case EGrowthStage::Flowering:
            return PlantConfig.FloweringMesh.LoadSynchronous();
        case EGrowthStage::Mature:
            return PlantConfig.MatureMesh.LoadSynchronous();

        default:
            return nullptr;
    }
}

// 评估土壤状态得到作物生长乘数
float UPlantBase::EvaluateSoilQuality(ESoilQuality SoilQuality) const
{
    return 1.0f;
}

// 评估土壤肥力得到作物生长乘数
float UPlantBase::EvaluateFertility(float SoilFertility) const
{
    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UPlantBase::EvaluateMoisture(float Moisture) const
{
    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UPlantBase::EvaluateTemperature(float Temperature) const
{
    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UPlantBase::EvaluateRadiation(int32 Radiation) const
{
    return 1.0f;
}

// 评估土壤水分得到作物生长乘数
float UPlantBase::EvaluateToxicity(float Toxicity) const
{
    return 1.0f;
}

void UPlantBase::SetPlantQuality()
{
    if(Stress < PlantDefaults::PREMIUM_QUALITY_STRESS_THRESHOLD)
    {
        CurrentQuality = EPlantQuality::Premium;
        return;
    }
    if(Stress < PlantDefaults::NORMAL_QUALITY_STRESS_THRESHOLD)
    {
        CurrentQuality = EPlantQuality::Normal;
        return;
    }
    if(Stress < PlantDefaults::WITHERED_QUALITY_STRESS_THRESHOLD)
    {
        CurrentQuality = EPlantQuality::Withered;
        return;
    }
}

FString UPlantBase::GetQualityText(EPlantQuality InPlantQuality) const
{
	switch(InPlantQuality)
	{
		case EPlantQuality::Premium:
			return TEXT("优质");
		case EPlantQuality::Normal:
			return TEXT("普通");
		case EPlantQuality::Withered:
			return TEXT("干瘪");

		default:
			return TEXT("");
	}
}