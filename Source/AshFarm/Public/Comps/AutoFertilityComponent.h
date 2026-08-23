// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AutoFertilityComponent.generated.h"

class APlantBed;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UAutoFertilityComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAutoFertilityComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "网格体", meta = (DisplayName = "施肥机网格体"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 是否开启自动施肥组件
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "是否开启自动施肥组件"))
	bool bEnabled = true;

	// 土壤肥力阈值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "土壤肥力阈值"))
	int32 FertilityThreshold = 20;

	// 每次施肥量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "每次施肥量"))
	int32 FertilityAmountPerTick = 1;

	// 内部存储量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "内部存储量"))
	int32 InternalStorage = 50;

	// 内部最大存储量
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "内部存储量"))
	int32 MaxStorage = 100;

	// 重新填充
	UFUNCTION(BlueprintCallable, Category = "操作", meta = (DisplayName = "重新填充"))
	void Refill(int32 Amount);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	TObjectPtr<APlantBed> Owner = nullptr;

	// 上次施肥是否失败，防止日志 Spam (水箱指针为空 或 水箱空了)
	bool bLastFailed = false;
};
