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
