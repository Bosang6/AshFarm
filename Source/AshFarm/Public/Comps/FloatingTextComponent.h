// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FloatingTextComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UFloatingTextComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFloatingTextComponent();

	// ==========================
	// 配置属性
	// ==========================

	// 文本大小缩放
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "漂浮文本组件", meta = (DisplayName = "文本大小缩放"))
	float TextScale = 1.0f;

	// 颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "漂浮文本组件", meta = (DisplayName = "颜色"))
	FColor TextColor = FColor::White;

	// 持续时间 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "漂浮文本组件", meta = (DisplayName = "持续时间"))
	float Duration = 1.0f;

	// 偏移高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "漂浮文本组件", meta = (DisplayName = "偏移高度"))
	float OffsetHeight = 150.0f;

	// ==========================
	// 功能函数
	// ==========================

	// 默认显示文本
	UFUNCTION(BlueprintCallable, Category = "漂浮文本组件", meta = (DisplayName = "默认显示文本"))
	void ShowTextDefault(const FString& InText);

	// 显示文本
	UFUNCTION(BlueprintCallable, Category = "漂浮文本组件", meta = (DisplayName = "显示文本"))
	void ShowText(const FString& InText, FColor InColor, float InScale, float InDuration, float InOffsetHeight);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
