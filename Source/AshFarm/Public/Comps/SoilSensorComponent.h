// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoilSensorComponent.generated.h"

class APlantBed;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API USoilSensorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoilSensorComponent();

	// 是否启用传感器
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "是否启用传感器"))
	bool bEnabled = true;

	// =============
	// 配置属性
	// =============

	// 土壤湿度最小值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤湿度最小值"))
	float MoistureMin = 20.0f;

	// 土壤肥力最小值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤肥力最小值"))
	float FertilityMin = 20.0f;

	// 土壤辐射最大值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤辐射最大值"))
	int32 RadiationMax = 0;

	// 土壤毒素最大值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "土壤", meta = (DisplayName = "土壤毒素最大值"))
	float ToxicityMax = 0.0f;

	// =============
	// 功能函数
	// =============

	// 获取土壤状态报告
	UFUNCTION(BlueprintCallable, Category = "传感器", meta = (DisplayName = "获取土壤状态报告"))
	FString GetStatusReport() const;

	// 土壤是否健康
	UFUNCTION(BlueprintCallable, Category = "传感器", meta = (DisplayName = "土壤是否健康"))
	bool IsSoilHealthy() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	TObjectPtr<APlantBed> Owner;
		
};
