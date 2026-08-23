// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/SoilSensorComponent.h"
#include "Actors/PlantBed.h"
#include "AshFarm.h"

// Sets default values for this component's properties
USoilSensorComponent::USoilSensorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;
	// ...
}


// Called when the game starts
void USoilSensorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Owner = GetOwner<APlantBed>();

	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("土壤传感器: Owner 无效"));
		return;
	}
}


// Called every frame
void USoilSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("土壤传感器: Owner 无效"));
		return;
	}

	if(!bEnabled)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("土壤传感器: 未开启"));
		return;
	}

	if(IsSoilHealthy()) return;

	DrawDebugString(
		Owner->GetWorld(),
		Owner->GetActorLocation(),
		GetStatusReport(),
		nullptr,
		FColor::White,
		DeltaTime
	);
}

// 获取土壤状态报告
FString USoilSensorComponent::GetStatusReport() const
{
	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("土壤传感器: Owner 无效"));
		return FString();
	}

	return FString::Printf(TEXT("[土壤传感器] 种植床: %d | 湿度: %f [%s] | 肥力: %f [%s]| 辐射值: %d [%s]| 毒性: %f [%s]"), 
				Owner->BedID,
				Owner->GetSoilMoisture(), Owner->GetSoilMoisture() >= MoistureMin ? TEXT("正常") : TEXT("过低"),
				Owner->GetSoilFertility(), Owner->GetSoilFertility() >= FertilityMin ? TEXT("正常") : TEXT("过低"),
				Owner->GetRadiationLevel(), Owner->GetRadiationLevel() <= RadiationMax ? TEXT("正常") : TEXT("过高"),
				Owner->GetToxcity(), Owner->GetToxcity() <= ToxicityMax ? TEXT("正常") : TEXT("过高")
			);
}

// 土壤是否健康
bool USoilSensorComponent::IsSoilHealthy() const
{
	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("土壤传感器: Owner 无效"));
		return false;
	}

	return (Owner->GetSoilMoisture() >= MoistureMin) &&
		(Owner->GetSoilFertility() >= FertilityMin) &&
		(Owner->GetToxcity() <= ToxicityMax) &&
		(Owner->GetRadiationLevel() <=  RadiationMax);
}
