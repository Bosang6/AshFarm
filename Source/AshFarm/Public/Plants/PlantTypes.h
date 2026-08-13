#pragma once

#include "CoreMinimal.h"
#include "Actors/PlantBedTypes.h"

#include "PlantTypes.generated.h"

namespace PlantDefaults
{
	static constexpr float PREMIUM_QUALITY_STRESS_THRESHOLD   	= 5.0f;  // 收成优质阈值
	static constexpr float NORMAL_QUALITY_STRESS_THRESHOLD  	= 15.0f; // 收成普通阈值
	static constexpr float WITHERED_QUALITY_STRESS_THRESHOLD  	= 30.0f; // 收成干瘪阈值
}

UENUM(BlueprintType)
enum class EGrowthStage : uint8
{
	Seedling    UMETA(DisplayName = "幼苗期"),
	Growing  	UMETA(DisplayName = "生长期"),
	Flowering 	UMETA(DisplayName = "开花期"),
	Mature  	UMETA(DisplayName = "成熟期"),
};

// Quality { 优质, 普通, 干瘪 }
UENUM(BlueprintType)
enum class EPlantQuality : uint8
{
	Premium     UMETA(DisplayName = "优质"),
	Normal  	UMETA(DisplayName = "普通"),
	Withered 	UMETA(DisplayName = "干瘪"),
};

// 植物生长环境 上下文(Context) / 参数(Params)
USTRUCT(BlueprintType)
struct FPlantGrowthContext
{	
	GENERATED_BODY()

	FPlantGrowthContext() : 
		SoilType(ESoilType::Loam),
		SoilQuality(ESoilQuality::Normal),
		Fertility(50.0f),
		Moisture(0.5f),
		Temperature(25.0f),
		RadiationLevel(0),
		Toxicity(0.0f),
		WindSpeed(0.0f),
		LightIntensity(0.5f)
	{

	}

	// ==============
	// 土壤数据
	// ==============

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "土壤类型"))
	ESoilType SoilType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "土壤品质"))
	ESoilQuality SoilQuality; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "土壤肥力"))
	float Fertility; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "土壤湿度"))
	float Moisture; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "环境温度"))
	float Temperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "辐射等级"))
	int32 RadiationLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "毒性等级"))
	float Toxicity;

	// ==============
	// 环境数据 
	// ==============

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "风速", ClampMin = "0.0", ClampMax = "10.0"))
	float WindSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "光照强度", ClampMin = "0.0", ClampMax = "1.0"))
	float LightIntensity;
};

// 植物环境值范围
USTRUCT(BlueprintType, meta = (DisplayName = "植物环境值范围"))
struct FPlantRange
{
	GENERATED_BODY()

	FPlantRange() :
		Min(0.0f),
		OptimalMin(0.4f),
		OptimalMax(0.6f),
		Max(1.0f)
	{

	}

	FPlantRange(float InMin, float InOptimalMin, float InOptimalMax, float InMax) :
		Min(InMin),
		OptimalMin(InOptimalMin),
		OptimalMax(InOptimalMax),
		Max(InMax)
	{

	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "最小值"))
	float Min = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "最优最小值"))
	float OptimalMin = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "最优最大值"))
	float OptimalMax = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长环境", meta = (DisplayName = "最大值"))
	float Max = 1.0f;

	// 结构体内的函数不能使用UFUNCTION
	float Evaluate(float Value) const
	{
		if(Value < Min || Value > Max)
		{
			return 0.0f;
		}

		if(Value >= OptimalMin && Value <= OptimalMax)
		{
			return 1.0f;
		}

		if(Value < OptimalMin)
		{
			float Range = OptimalMin - Min;
			if(Range <= 0) 
			{
				return 1.0f;
			}

			float Alpha = (Value - Min) / Range;

			return FMath::Lerp(0.0f, 1.0f, Alpha);
		}

		if(Value > OptimalMax)
		{
			float Range = Max - OptimalMax;
			if(Range <= 0) 
			{
				return 1.0f;
			}

			float Alpha = (Value - OptimalMax) / Range;

			return FMath::Lerp(1.0f, 0.0f, Alpha);
		}

		return 1.0f;
	}
}; 

// 植物配置
USTRUCT(BlueprintType, meta = (DisplayName = "植物配置"))
struct FPlantConfig : public FTableRowBase
{
	GENERATED_BODY()

	FPlantConfig() :
		PlantName(TEXT("未命名植物")),
		Description(FText::FromString(TEXT("未命名植物的描述"))),
		GrowthSpeed(1.0f),
		WaterConsumption(0.01f),
		FertilityConsumption(0.01f),
		MatureProgress(100.0f),
		Sensitivity(1.0f)
	{

	}

	// ====================
	// 植物属性 配置 Config
	// ====================

	// 植物名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物", meta = (DisplayName = "植物名称"))
	FString PlantName;

	// 植物描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物", meta = (DisplayName = "植物描述"))
	FText Description;

	// 生长速度 (归一化)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长数据", meta = (DisplayName = "生长速度"))
	float GrowthSpeed = 1.0f;

	// 每秒耗水量 (归一化)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长数据", meta = (DisplayName = "每秒耗水量"))
	float WaterConsumption = 0.01f;

	// 每秒肥料消耗
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长数据", meta = (DisplayName = "每秒肥料消耗"))
	float FertilityConsumption = 0.01f;

	// 成熟所需进度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长数据", meta = (DisplayName = "成熟所需进度"))
	float MatureProgress = 100.0f;

	// 植物敏感度 植物的敏感度会影响逆境值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|生长数据", meta = (DisplayName = "植物敏感度"))
	float Sensitivity = 1.0f;

	// ====================
	// 环境需求
	// ====================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "偏好土壤类型"))
	ESoilType PreferredSoilType = ESoilType::Loam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "肥力范围"))
	FPlantRange FertilityRange = FPlantRange(0.0f, 10.0f, 90.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "湿度范围"))
	FPlantRange MoistureRange = FPlantRange(0.0f, 0.3f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "温度范围"))
	FPlantRange TemperatureRange = FPlantRange(0.0f, 20.0f, 30.0f, 40.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "辐射范围"))
	FPlantRange RadiationRange = FPlantRange(0.0f, 0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "毒性范围"))
	FPlantRange ToxicityRange = FPlantRange(0.0f, 0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "风速范围"))
	FPlantRange WindSpeedRange = FPlantRange(0.0f, 0.0f, 3.0f, 3.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|环境需求", meta = (DisplayName = "光照范围"))
	FPlantRange LightIntensityRange = FPlantRange(0.3f, 0.8f, 1.0f, 1.0f);

	// ====================
	// 植物外观 配置 Config
	// ====================

	// 幼苗期网格体
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|外观", meta = (DisplayName = "幼苗期网格体"))
	TSoftObjectPtr<UStaticMesh> SeedlingMesh;

	// 生长期网格体
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|外观", meta = (DisplayName = "生长期网格体"))
	TSoftObjectPtr<UStaticMesh> GrowthMesh;

	// 开花期网格体
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|外观", meta = (DisplayName = "开花期网格体"))
	TSoftObjectPtr<UStaticMesh> FloweringMesh;

	// 成熟期网格体
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物|外观", meta = (DisplayName = "成熟期网格体"))
	TSoftObjectPtr<UStaticMesh> MatureMesh;

};
