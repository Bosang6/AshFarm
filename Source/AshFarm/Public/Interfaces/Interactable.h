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

	// 执行交互
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "交互")
	void OnInteract();
};
