// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/PlantBedTypes.h"
#include "Interfaces/Interactable.h"
#include "Building.h"
#include "PlantBed.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;
class UPlantBase;
class UProgressBarComponent;

#define GROWTH_SPEED_SALINE   0.0f
#define GROWTH_SPEED_POOR     0.5f
#define GROWTH_SPEED_NORMAL   1.0f
#define GROWTH_SPEED_FERTILE  1.5f

UCLASS(ClassGroup = "AshFarm|种植系统")
class ASHFARM_API APlantBed : public ABuilding
{
	GENERATED_BODY()
	
public:	
	APlantBed();

	// 进度条组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "进度条组件", meta = (DisplayName = "进度条组件"))
	TObjectPtr<UProgressBarComponent> ProgressBarComponent;

	// 目前种植的作物类型
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category = "种植", meta = (DisplayName = "当前作物类型"))
	TObjectPtr<UPlantBase> CurrentPlant;

	
	// 土壤状态
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "土壤", meta = (DisplayName = "土壤状态"))
	ESoilQuality SoilQuality = ESoilQuality::Normal;

	// 土壤类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤类型"))
	ESoilType SoilType = ESoilType::Loam;

	// 土壤肥力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤肥力"))
	float SoilFertility = PlantBedDefaults::DEFAULT_SOIL_FERTILITY;

	// 最大土壤肥力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "最大肥力"))
	float MaxSoilFertility = PlantBedDefaults::DEFAULT_SOIL_MAX_FERTILITY;

	// 土壤湿度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤湿度"))
	float Moisture = 50.0f;

	// 最大土壤湿度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "最大土壤湿度"))
	float MaxMoisture = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤温度"))
	float Temperature = 26.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "辐射等级"))
	int32 RadiationLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "毒素"))
	float Toxicity = 0.0f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "ID", ClampMin = "0", ClampMax = "5"))
	int32 BedID = 0;

	// 相邻种植床
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "种植", meta = (DisplayName = "邻居种植床"))
	TSet<TObjectPtr<APlantBed>> NeighborBeds;

	// 相邻种植床检测距离
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "种植", meta = (DisplayName = "相邻种植床检测距离"))
	float NeighborDetectDistance = 300.0f;

	// =============
	// 功能函数
	// =============

	// 获取土壤肥力
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取土壤肥力"))
	float GetSoilFertility() const; // const 表示这个函数不会修改类的变量

	// 设置土壤肥力
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "设置土壤肥力"))
	void SetSoilFertility(float Fertility);

	// 获取土壤湿度
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取土壤湿度"))
	float GetSoilMoisture() const;

	// 获取土壤辐射等级
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取土壤辐射等级"))
	int32 GetRadiationLevel() const;

	// 获取土壤毒性
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取土壤毒性"))
	float GetToxcity() const;

	// 浇水
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "浇水"))
	void ReceiveMoisture(float InMoisture);

	// 施肥
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "施肥"))
	void ReceiveFertility(float InFertility);

	// 获取生长速度
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取生长速度"))
	float GetGrowthSpeed() const;

	// 根据土壤品质获取土壤湿度流失率
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "根据土壤品质获取土壤湿度流失率"))
	float GetMoistureLossRateBySoilQuality() const;

	// 根据土壤类型获取土壤肥力流失率
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "根据土壤类型获取土壤肥力流失率"))
	float GetFertilityLossRateBySoilType() const;

	// 根据土壤类型获取土壤湿度流失率
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "根据土壤类型获取土壤湿度流失率"))
	float GetMoistureLossRateBySoilType() const;

	// 获取土壤品质文本
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取土壤品质文本"))
	FString GetSoilQualityText(ESoilQuality Quality) const;

	// 获取土壤类型文本
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取土壤类型文本"))
	FString GetSoilTypeText(ESoilType InSoilType) const;

	//获取植物类型枚举中文
	// UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取土壤状态文本"))
	// FString GetSoilCropName(ECropType Crop) const;

	// 从AAcotr继承的函数，类似蓝图中的构造脚本，每次拖动Actor都会执行
	virtual void OnConstruction(const FTransform& Transform) override;

	// 清除死亡植物
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "清除死亡植物"))
	void ClearDeadPlant();

	// 收获植物
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "收获植物"))
	void Harvest();

	// 打印状态
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "打印状态"))
	void PrintState();

	// ==============
	// 回调函数
	// ==============

	// 任何的回调函数都必须用 UFUNCTION 修饰
	UFUNCTION(meta = (DisplayName = " 当植物成熟时"))
	void OnPlantMatured(UPlantBase* Plant); //回调函数的前面必须与在 PlantBase.h 用宏声明的委托一致

	// ==============
	// 统计数据
	// ==============

	// 土壤状态转换计数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "土壤", meta = (DisplayName = "状态转换计数"))
	int32 TransitionCount = 0;

	// 获取所有种植床实例的数量
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "获取所有种植床实例的数量"))
	static int32 GetTotalCount();

	// ========== 接口实现 ==============
	virtual void OnSelected_Implementation() override;
	virtual void OnInteract_Implementation() override;
	// =================================

protected:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (DisplayName = "植物网格体"))
	TObjectPtr<UStaticMeshComponent> PlantMesh;

	// 种植点组件
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "PlantingPoint")
	TObjectPtr<USceneComponent> PlantingPoint;

	virtual void BeginPlay() override;

	// 更新土壤肥力
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "更新土壤肥力"))
	void UpdateSoilQuality();

	// 更新土壤水分
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "更新土壤水分"))
	void SetMoistureLossPerSecond(float DeltaTime);

	// 土壤损失肥力
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "土壤损失肥力"))
	void SetFertilityLossPerSecond(float DeltaTime);

	// 更新植物网格体
	UFUNCTION(BlueprintCallable, Category = "土壤", meta = (DisplayName = "更新植物网格体"))
	void UpdatePlantMesh();

	// 初始化相邻种植床
	UFUNCTION(BlueprintCallable, Category = "种植", meta = (DisplayName = "初始化相邻种植床"))
	void InitNeighborBeds();


public:	
	virtual void Tick(float DeltaTime) override;

	virtual void EndPlay(const EEndPlayReason::Type EndplayReason) override;

private:
	// 统计所有APlantBed实例数量
	static int32 TotalCount;

	

};
