// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressBarComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UProgressBarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProgressBarComponent();

	// ==============
	// 运行时
	// ==============

	// 当前进度
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "进度条组件", meta = (DisplayName = "当前进度"))
	float CurrentProgress = 0.0f;

	// ==============
	// 配置属性
	// ==============

	// 相对Actor头顶的偏移高度，用于展示进度条
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "进度条组件", meta = (DisplayName = "偏移高度"))
	float HeightOffset = 150.0f;

	// 进度条颜色：当100%进度时显示颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "进度条组件", meta = (DisplayName = "满进度颜色"))
	FColor FullColor;

	// 进度条颜色：当0%进度时显示颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "进度条组件", meta = (DisplayName = "空进度颜色"))
	FColor EmptyColor;

	// 默认进度颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "进度条组件", meta = (DisplayName = "默认进度颜色"))
	FColor NormalColor;

	// ==============
	// 功能函数
	// ==============

	// 设置进度
	UFUNCTION(BlueprintCallable, Category = "进度条组件", meta = (DisplayName = "设置进度"))
	void SetProgress(float Value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TObjectPtr<AActor> Owner;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 绘制进度条
	void DrawProgressBar();

	// 绘制闪烁点
	void DrawFlashingDot();

	// 根据当前进度获取颜色
	FColor GetCurrentColor();

		
};
