// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
// 告诉UE这是个接口, U就是给UE反射的
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASHFARM_API IInteractable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// 被选中时调用
	// BlueprintNativeEvent: 蓝图可覆盖C++默认代码
	// 与传统C++不同的是，UE接口函数不需要在前面添加virtual
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	void OnSelected();

	// 被取消选择时
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	void OnUnselected();

	// 执行交互
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	void OnInteract();

	// BlueprintNativeEvent：C++ 默认实现 + 蓝图可覆盖
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	bool IsInteractable() const;
	virtual bool IsInteractable_Implementation() const { return true; }  // ← 内联默认实现

	// 我（作为 SelectedActor）能不能对 Target 下手？
	// 默认实现 false：默认情况下，所有对象都不能对别人下手（比如一块石头、一个仓库）。
	// 只有水箱、堆肥区、NPC 等"工具型 Actor"才覆盖为 true
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	bool CanInteractWith(const AActor* Target) const;
	virtual bool CanInteractWith_Implementation(const AActor* Target) const { return false; }

	// 我（作为 SelectedActor）对 Target 执行操作
	// 不强制所有类实现
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	void OnInteractWith(AActor* Target);

	// 当我作为 SelectedActor 被选中时，UI 应该提示什么？
	// 选中水箱后，鼠标旁边显示"点击浇水"而不是"点击收获"。
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	FString GetAsSelectedHint() const;
	virtual FString GetAsSelectedHint_Implementation() const{ return TEXT(""); }
};
