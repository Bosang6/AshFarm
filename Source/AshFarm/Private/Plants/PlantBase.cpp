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

// 获取所有植物名称 （供编辑器下拉菜单使用）
TArray<FName> UPlantBase::GetPlantNames() const
{
    TArray<FName> PlantNames;

    if(UPlantBase::PlantDataTable)
    {
        PlantNames = UPlantBase::PlantDataTable->GetRowNames();
    }

    return PlantNames;
}

// 生长
void UPlantBase::Grow(float DeltaTime, const FPlantGrowthContext& Context
    )
{
    // 植物死亡则不生长
    if(IsDead())
    {
        return;
    }

    TotalGrowthTime += DeltaTime;

    if(TotalGrowthTime >= PlantConfig.MaxGrowthTime)
    { 
        GrowthStage = EGrowthStage::Death;
        OnDeath();

        return;
    }

    // 已成熟就不生长
    if(bIsMature)
    {
        return;
    }

    float EvaluatedMulti = EvaluateSoilQuality(Context.SoilQuality) *
        EvaluateSoilType(Context.SoilType) *
        EvaluateFertility(Context.Fertility) * 
        EvaluateMoisture(Context.Moisture) *
        EvaluateTemperature(Context.Temperature) *
        EvaluateRadiation(Context.RadiationLevel) *
        EvaluateToxicity(Context.Toxicity) *
        EvaluateWindSpeed(Context.WindSpeed) *
        EvaluateLightIntensity(Context.LightIntensity);

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

// 获取生长时间
FString UPlantBase::GetGrowthTimeText() const
{
    int32 Hours = 0;
    int32 Minutes = 0;
    int32 Seconds = 0;

    int32 TotalSeconds = FMath::FloorToInt(TotalGrowthTime);

    if(TotalSeconds / 3600 > 0)
    {
        Hours = TotalSeconds / 3600;
        TotalSeconds %= 3600;
    }
    if(TotalSeconds / 60 > 0)
    {
        Minutes = TotalSeconds / 60;
        TotalSeconds %= 60;
    }
    Seconds = TotalSeconds;

    if(Hours > 0)
    {
        return FString::Printf(TEXT("%d时%d分%d秒"), Hours, Minutes, Seconds);
    }
    else if(Minutes > 0)
    {
        return FString::Printf(TEXT("%d分%d秒"), Minutes, Seconds);
    }
    else
    {
        return FString::Printf(TEXT("%d秒"), Seconds);
    }
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
    // Death 是终态，不允许被生长阶段计算覆盖
    if(IsDead())
    {
        return;
    }

    float GrowthProgressRatio = GetGrowthProgressPercentage();

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

// 获取生长进度百分比
float UPlantBase::GetGrowthProgressPercentage() const
{
    return GrowthProgress / PlantConfig.MatureProgress;
}

// 当成熟时调用
void UPlantBase::OnMature()
{
    SetPlantQuality();
    UE_LOG(A_LogAshFarm, Warning, TEXT("收获了 %s (品质: %s)"), *GetPlantName(), *GetQualityText(CurrentQuality));
}

// 当死亡时调用
void UPlantBase::OnDeath()
{
    UE_LOG(A_LogAshFarm, Warning, TEXT("植物: %s 死亡了"), *GetPlantName());
}

// 是否死亡
bool UPlantBase::IsDead() const 
{
    return GrowthStage == EGrowthStage::Death; 
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

// 收获产量计算
float UPlantBase::CalculateHarvest() const
{
    // 植物死亡则不收获
    if(GrowthStage == EGrowthStage::Death)
    {
        return 0.0f;
    }

    // 计算品质倍率
    float QualityMulti = 1.0f;
    switch (CurrentQuality)
    {
        case EPlantQuality::Premium:
            QualityMulti = PlantDefaults::PREMIUM_QUALITY_HARVEST_MULTI;
            break;
        case EPlantQuality::Normal:
            QualityMulti = PlantDefaults::NORMA_QUALITY_HARVEST_MULTI;
            break;
        case EPlantQuality::Withered:
            QualityMulti = PlantDefaults::WITHERED_QUALITY_HARVEST_MULTI;
            break;
        
        default:
            QualityMulti = PlantDefaults::NORMA_QUALITY_HARVEST_MULTI;
    }

    return PlantConfig.BaseHarvestAmount * QualityMulti;
}