// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GreenHouseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UGreenHouseComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGreenHouseComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "网格体", meta = (DisplayName = "玻璃罩网格体"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 是否开启保湿
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "是否启用"))
	bool bEnabled = true;

	// 湿度流失减少速率
	// 0.5 = 流失率减半 | 0.0 = 完全不流失 | 1.0 = 没有保湿效果
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "状态", meta = (DisplayName = "是否启用"))
	float MoistureLossReductionRate = 0.5f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
