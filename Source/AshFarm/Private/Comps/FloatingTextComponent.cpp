// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/FloatingTextComponent.h"
#include "AshFarm.h"

// Sets default values for this component's properties
UFloatingTextComponent::UFloatingTextComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFloatingTextComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFloatingTextComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 默认显示文本
void UFloatingTextComponent::ShowTextDefault(const FString& InText)
{
	ShowText(InText, TextColor, TextScale, Duration, OffsetHeight);
}

// 显示文本
void UFloatingTextComponent::ShowText(const FString& InText, FColor InColor, float InScale, float InDuration, float InOffsetHeight)
{
	TObjectPtr<AActor> Owner = GetOwner();

	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Error, TEXT("[漂浮本文组件] Owner 无效"));
		return;
	}

	FVector TextLocation = Owner->GetActorLocation() + FVector(0.0f, 0.0f, OffsetHeight);

	DrawDebugString(
		Owner->GetWorld(),
		TextLocation,
		InText,
		nullptr,
		InColor,
		InDuration,
		true,
		InScale
	);
}