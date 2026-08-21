// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HighlightComponent.generated.h"


UCLASS( ClassGroup="AshFarm | 组件", Blueprintable, meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UHighlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHighlightComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TObjectPtr<AActor> Owner;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 设置高亮状态
	UFUNCTION(BlueprintCallable, Category = "高亮组件", meta = (DisplayName = "设置高亮状态"))
	void SetHighlight(bool bHighlight);	
};
