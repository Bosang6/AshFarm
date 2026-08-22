// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/ProgressBarComponent.h"
#include "AshFarm.h"

// Sets default values for this component's properties
UProgressBarComponent::UProgressBarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
	// ...
}


// Called when the game starts
void UProgressBarComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(!IsValid(GetOwner()))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("进度条组件: Owner 无效"));
		return;
	}
	Owner = GetOwner();	
}


// Called every frame
void UProgressBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawProgressBar();

	if(CurrentProgress >= 1.0f)
	{
		DrawFlashingDot();
	}
}

// 设置进度
void UProgressBarComponent::SetProgress(float Value)
{
	CurrentProgress = FMath::Clamp(Value, 0.0f, 1.0f);
}

// 绘制进度条
void UProgressBarComponent::DrawProgressBar()
{
	if(!IsValid(Owner)) return;

	FVector BarLocation = Owner->GetActorLocation() + FVector(0.0f, 0.0f, HeightOffset);

	FString InText = FString::Printf(TEXT("%.0f%%"), CurrentProgress * 100.0f);

	// TODO: 之后改成 Widget UI 显示文本
	DrawDebugString(
		Owner->GetWorld(),
		BarLocation,
		InText,
		nullptr,
		GetCurrentColor(),
		0.1f,
		true,
		1.0f
	);
}

// 根据当前进度获取颜色
FColor UProgressBarComponent::GetCurrentColor()
{
	if(CurrentProgress >= 1.0f)
	{
		return FullColor;
	}
	else if(CurrentProgress <= 0.0f)
	{
		return EmptyColor;
	}
	else
	{
		return NormalColor;
	}
}

// 绘制闪烁点
void UProgressBarComponent::DrawFlashingDot()
{
	if(!IsValid(Owner)) return;

	bool bFlash = FMath::Fmod(Owner->GetWorld()->GetTimeSeconds(), 1.0f) < 0.5f;

	if(!bFlash) return;

	DrawDebugPoint(
		Owner->GetWorld(),
		Owner->GetActorLocation() + FVector(0.0f, 0.0f, HeightOffset + 30.0f),
		5.0f,			// 点的大小
		FColor::Red,
		false,
		0.1f
	);
}
