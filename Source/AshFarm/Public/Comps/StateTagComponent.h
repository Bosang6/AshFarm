// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateTagComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UStateTagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateTagComponent();

	// 添加标签
	UFUNCTION(BlueprintCallable, Category = "状态标签组件", meta = (DisplayName = "添加标签"))
	void AddTag(FName Tag);

	// 移除标签
	UFUNCTION(BlueprintCallable, Category = "状态标签组件", meta = (DisplayName = "移除标签"))
	void RemoveTag(FName Tag);

	// 检查标签是否存在
	UFUNCTION(BlueprintCallable, Category = "状态标签组件", meta = (DisplayName = "检查标签是否存在"))
	bool HasTag(FName Tag) const;

	// 清除所有标签
	UFUNCTION(BlueprintCallable, Category = "状态标签组件", meta = (DisplayName = "清除所有标签"))
	void ClearAllTags();

	// 获取当前激活的标签字符串
	UFUNCTION(BlueprintCallable, Category = "状态标签组件", meta = (DisplayName = "获取当前激活的标签字符串"))
	FString GetTagsAsString() const;

	// 获取标签的数量
	UFUNCTION(BlueprintCallable, Category = "状态标签组件", meta = (DisplayName = "获取标签的数量"))
	int32 GetTagsCount() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// 当前激活的标签
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "状态标签组件", meta = (DisplayName = "当前激活的标签"))
	TSet<FName> ActiveTags;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
