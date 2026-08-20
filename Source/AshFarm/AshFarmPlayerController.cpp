// Copyright Epic Games, Inc. All Rights Reserved.

#include "AshFarmPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AshFarmCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Actors/HandPump.h"
#include "Inventory/Inventory.h"
#include "Actors/PlantBed.h"
#include "Actors/HandPump.h"
#include "AshFarm.h"

AAshFarmPlayerController::AAshFarmPlayerController()
{
	bIsTouch = false;
	bMoveToMouseCursor = false;

	// create the path following comp
	PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(TEXT("Path Following Component"));

	// configure the controller
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FollowTime = 0.f;
}

void AAshFarmPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Only set up input on local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			// Setup mouse input events
			/*
				BindAction参数介绍:
				1. InputAction, 在Input中定义的Action
				2. 输入方式：Triggered、Started、Ongoing、Canceled、Completed
				3.
				4. 输入对应的回调函数
			*/
			EnhancedInputComponent->BindAction(MoveCameraAction, ETriggerEvent::Triggered, this, &AAshFarmPlayerController::MoveCamera);
			EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AAshFarmPlayerController::ZoomCamera);
			EnhancedInputComponent->BindAction(SelectClickAction, ETriggerEvent::Triggered, this, &AAshFarmPlayerController::SelectClick);
		}
		else
		{
			UE_LOG(LogAshFarm, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
		}
	}
}

// 移动相机
void AAshFarmPlayerController::MoveCamera(const FInputActionValue& Value)
{
	FVector2D InputVector = Value.Get<FVector2D>();

	// get the forward input component vector
	FRotator ForwardRot = GetControlRotation();
	ForwardRot.Pitch = 0.0f;

	// get the right input component vector
	FRotator RightRot = GetControlRotation();
	RightRot.Pitch = 0.0f;
	RightRot.Roll = 0.0f;
	
	// GetPawn() ：获取当前控制器所控制的玩家
	TObjectPtr<APawn> ControlledPawn = GetPawn();
	
	// add the forward input
	if (ControlledPawn)
	{
		ControlledPawn->AddMovementInput(ForwardRot.RotateVector(FVector::ForwardVector), InputVector.X + InputVector.Y);

		// add the right input
		ControlledPawn->AddMovementInput(RightRot.RotateVector(FVector::RightVector), InputVector.X - InputVector.Y);
	}
}

// 缩放相机
void AAshFarmPlayerController::ZoomCamera(const FInputActionValue& Value)
{
	float ZoomDelta = Value.Get<float>();

	// Cast to
	TObjectPtr<AAshFarmCharacter> AshFarmCharacter = Cast<AAshFarmCharacter>(GetPawn());

	if(AshFarmCharacter)
	{
		AshFarmCharacter->ZoomCamera(ZoomDelta);
	}
}

// 选择点击
void AAshFarmPlayerController::SelectClick(const FInputActionValue& Value)
{
	TObjectPtr<AActor> ClickedActor = LineTrace();

	if(!ClickedActor)
	{
		return;
	}

	if(IsValid(SelectedActor) && ClickedActor == SelectedActor)
	{
		// 交互
		if(ClickedActor->Implements<UInteractable>())
		{
			IInteractable::Execute_OnInteract(ClickedActor);
			return;
		}
	}

	// 判断对象是否实现了 UInteractable 这个接口类
	if(ClickedActor->Implements<UInteractable>())
	{
		// 若实现，则执行对象已实现的 OnSelected 接口函数
		IInteractable::Execute_OnSelected(ClickedActor);

		// 选中新的 Actor
		SelectedActor = ClickedActor;
	}
}

// 射线检测
TObjectPtr<AActor> AAshFarmPlayerController::LineTrace()
{
	// 把鼠标屏幕坐标转成世界空间坐标的位置和方向
	// Deproject = 2D屏幕 -> 3D空间
	FVector WorldLocation, WorldDirection;
	if(!this->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		return nullptr;
	}

	// 射线起点和射线末端
	FVector TraceStart = WorldLocation;
	FVector TraceEnd = WorldLocation + WorldDirection * 5000.0f;

	// 碰撞查询参数: 忽略自己
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// 执行射线检测
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,			// 输出：命中结果
		TraceStart,			// 输入：射线起点
		TraceEnd,			// 输入：射线终点
		ECC_Visibility,		// 碰撞通道：Visibility
		QueryParams			// 查询参数
	);

	// 调试画线
	if(bDebug)
	{
		// 参数：世界、起点、终点、颜色、是否持久(false=只画一帧)、持续时间、深度优先级、线宽
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, 10.0f, 0, 2.0f);
		if(bHit)
		{
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Yellow, false, 10.0f);
		}
	}


	if(bHit && IsValid(HitResult.GetActor()))
	{
		UE_LOG(LogAshFarm, Error, TEXT("点击到的Actor: %s"), *HitResult.GetActor()->GetName());
		return HitResult.GetActor();
	}
	else
	{
		return nullptr;
	}
}