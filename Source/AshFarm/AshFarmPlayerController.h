// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
//#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "AshFarmPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UPathFollowingComponent;
struct FInputActionValue; // #include "InputActionValue.h"


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Player controller for a top-down perspective game.
 *  Implements point and click based controls
 */
UCLASS(abstract)
class AAshFarmPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Component used for moving along a NavMesh path. */
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	TObjectPtr<UPathFollowingComponent> PathFollowingComponent;

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, Category="Input")
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UNiagaraSystem> FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 输入操作: 相机移动
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveCameraAction;

	// 输入操作: 缩放相机
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ZoomAction;

	// 输入操作: 选择点击
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SelectClickAction;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/** Set to true if we're using touch input */
	uint32 bIsTouch : 1;

	/** Time that the click input has been pressed */
	float FollowTime = 0.0f;

public:

	/** Constructor */
	AAshFarmPlayerController();

	// =========== 组件安装规则 =====================
	UPROPERTY(EditAnywhere, Category = "组件功能", meta = (DisplayName = "组件安装规则表"))
	TObjectPtr<UDataTable> InstallRuleTable;

	// 选中的Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "选择", meta = (DisplayName = "选中的Actor"))
	TObjectPtr<AActor> SelectedActor;

	// 调试：是否显示调试线
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (DisplayName = "显示调试线"))
	bool bDebug = false;

	// 安装功能: 添加组件
	UFUNCTION(BlueprintCallable, Category = "功能组件", meta = (DisplayName = "安装组件"))
	void InstallComponentOnSelected(TSubclassOf<UActorComponent> ComponentClass);

protected:

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	// 射线检测
	TObjectPtr<AActor> LineTrace();

	void MoveCamera(const FInputActionValue& Value);  // 移动相机
	void ZoomCamera(const FInputActionValue& Value);  // 缩放相机
	void SelectClick(const FInputActionValue& Value); // 选择点击

	// 清除选中
	void ClearSelection();
	
	
};


