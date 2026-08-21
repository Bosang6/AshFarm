// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/CoolingComponent.h"
#include "AshFarm.h"

// Sets default values for this component's properties
UCoolingComponent::UCoolingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;
	// ...
}


// Called when the game starts
void UCoolingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCoolingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!bEnabled) return;

	// ==== 自然散热 ======
	if(CurrentHeat > 0.0f && bEnabledCooling)
	{
		CurrentHeat -= CooldownRate * DeltaTime;
		CurrentHeat = FMath::Clamp(CurrentHeat, 0.0f, MaxHeat);

		if(bOverHeat && CurrentHeat <= 0.0f)
		{
			bOverHeat = false;
			UE_LOG(A_LogAshFarm, Warning, TEXT("%s 已冷却, 可以继续使用"), *GetOwner()->GetName());
		}
	}
}

// 增加热量
void UCoolingComponent::AddHeat()
{
	if(!bEnabled) return;

	CurrentHeat += HeatPerUse;
	CurrentHeat = FMath::Clamp(CurrentHeat, 0.0f, MaxHeat);
	if(CurrentHeat >= MaxHeat)
	{
		bOverHeat = true;
		UE_LOG(A_LogAshFarm, Warning, TEXT("%s 已过热, 请等待冷却"), *GetOwner()->GetName());
	}
}

// 获取当前热量占比
float UCoolingComponent::GetHeatPercentage() const
{
	ensure(MaxHeat > 0.0f);

	if(MaxHeat == 0.0f)
	{
		return 0.0f;
	}

	return CurrentHeat / MaxHeat;
}