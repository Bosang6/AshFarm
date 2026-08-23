// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AutoWateringComponent.generated.h"

class AWaterTank;
class APlantBed;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UAutoWateringComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoWateringComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "网格体", meta = (DisplayName = "浇水机网格体"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 是否开启自动浇水组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "是否开启自动浇水组件"))
	bool bEnabled = true;

	// 土壤湿度阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "土壤湿度阈值"))
	float MoistureThreshold = 20.0f;

	// 每次浇水量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "每次浇水量"))
	float WaterAmountPerTick = 0.5f;

	// 水源水箱
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "水源水箱"))
	TObjectPtr<AWaterTank> WaterSource = nullptr;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	TObjectPtr<APlantBed> Owner = nullptr;
};
