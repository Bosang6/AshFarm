// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Actors/PlantBedTypes.h"
#include "Plants/PlantTypes.h"

#include "PlantBase.generated.h"


/**
 * 植物基类
 * 
 * BlueprintType: 蓝图类型，可以在蓝图中使用，作为蓝图变量
 * Blueprintable: 允许使用这个C++类作为蓝图类的父类
 */

class PlantBed;

namespace PlantDefaults
{
	const float GROWTH_PROGRESS_THRESHOLD    = 0.3f; // 生长期网格体阈值
	const float FLOWERING_PROGRESS_THRESHOLD = 0.6f; // 开花期网格体阈值
}

UCLASS(Abstract, BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew,meta = (DisplayName = "植物基类"))
class ASHFARM_API UPlantBase : public UObject
{
	GENERATED_BODY()

public:

	UPlantBase() = default;

	~UPlantBase() = default;

	// PostInitProperties
	// 当构造完成后调用
	virtual void PostInitProperties() override;

	// PostEditChangeProperty 
	// 编辑器专用回调, 在Detail面板中, 任何数据被修改都会被调用。用于更新编辑时状态，便于策划观察。
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	// ====================
	// 植物配置数据表 
	// ====================

	static TObjectPtr<UDataTable> PlantDataTable;
	static void LoadPlantDataTable();

	// 通过 RowName行命名 从植物配置表加载植物属性
	bool LoadFromPlantDataTable();

	// ====================
	// 植物属性 配置 Config
	// ====================

	// 行命名
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物", meta = (DisplayName = "行命名", GetOptions = "GetPlantNames"))
	FName RowName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "植物", meta = (DisplayName = "植物属性配置"))
	FPlantConfig PlantConfig;

	// 获取所有植物名称 （供编辑器下拉菜单使用）
	UFUNCTION(CallInEditor)
	TArray<FName> GetPlantNames() const;

	// ====================
	// 运行时状态 RuntimeState
	// ====================

	// 生长进度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "植物|生长数据", meta = (DisplayName = "生长进度"))
	float GrowthProgress = 0.0f;

	// 植物生长阶段
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "植物|外观", meta = (DisplayName = "植物生长阶段"))
	EGrowthStage GrowthStage = EGrowthStage::Seedling;

	// 植物逆境值，影响作物收成 
	//（生长过程中，只要环境综合倍率(环境适应程度)低于 1.0（作物在受罪），就按差距累积逆境值）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "植物|生长数据", meta = (DisplayName = "植物逆境值"))
	float Stress = 0.0f;

	// 收成品质
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "植物|生长数据", meta = (DisplayName = "收成品质"))
	EPlantQuality CurrentQuality = EPlantQuality::Premium;
	
	// 是否已经成熟
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "植物|生长数据", meta = (DisplayName = "是否成熟"))
	bool bIsMature = false;

	// ====================
	// 植物功能函数
	// ====================

	// 获取植物名称
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "获取植物名称"))
	virtual FString GetPlantName() const { return PlantConfig.PlantName; };

	// 获取植物简介
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "获取植物描述"))
	virtual FText GetDescription() const { return PlantConfig.Description; };

	// 根据生长进度获取网格体
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "根据生长进度获取网格体"))
	virtual UStaticMesh* GetStageMesh() const;

	// 生长
	UFUNCTION(BlueprintCallable, Category = "植物")
	virtual void Grow(
		UPARAM(DisplayName = "时间间隔") float DeltaTime,
		const FPlantGrowthContext& Context
	);

	// 获取当前生长阶段
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "获取当前生长阶段"))
	virtual EGrowthStage GetGrowthStage() const { return GrowthStage; };

	// 获取当前生长阶段文字
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "获取当前生长阶段文字"))
	virtual FString GetGrowthStageText(EGrowthStage InGrowthStage) const;

	// 设置生长状态, 判断的阈值还是用之前一样的
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "设置生长状态"))
	void SetGrowthStage();

	// 评估土壤状态得到作物生长乘数
	virtual float EvaluateSoilQuality(ESoilQuality SoilQuality) const { return 1.0f; };

	virtual float EvaluateSoilType(ESoilType SoilType) const { return PlantConfig.PreferredSoilType == SoilType ? 1.2f : 1.0f; };

	// 评估土壤肥力得到作物生长乘数
	virtual float EvaluateFertility(float SoilFertility) const { return PlantConfig.FertilityRange.Evaluate(SoilFertility); }; 

	// 评估土壤水分得到作物生长乘数
	virtual float EvaluateMoisture(float Moisture) const { return PlantConfig.MoistureRange.Evaluate(Moisture); };

	// 评估温度得到作物生长乘数
	virtual float EvaluateTemperature(float Temperature) const { return PlantConfig.TemperatureRange.Evaluate(Temperature); };

	// 评估辐射等级得到作物生长乘数
	virtual float EvaluateRadiation(int32 Radiation) const { return PlantConfig.RadiationRange.Evaluate(Radiation); };

	// 评估毒素得到作物生长乘数
	virtual float EvaluateToxicity(float Toxicity) const { return PlantConfig.ToxicityRange.Evaluate(Toxicity); };

	// 评估风速得到作物生长乘数
	virtual float EvaluateWindSpeed(float WindSpeed) const { return PlantConfig.WindSpeedRange.Evaluate(WindSpeed); };

	// 评估光照得到作物生长乘数
	virtual float EvaluateLightIntensity(float LightIntensity) const { return PlantConfig.LightIntensityRange.Evaluate(LightIntensity); };

	// 当成熟时调用
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "当成熟时"))
	virtual void OnMature();

	// 设置收成品质
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "设置收成品质"))
	void SetPlantQuality();

	// 获取作物品质
	UFUNCTION(BlueprintCallable, Category = "植物", meta = (DisplayName = "获取作物品质"))
	FString GetQualityText(EPlantQuality InPlantQuality) const;
};
