// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/WaterTank.h"
#include "Actors/PlantBed.h"

bool AWaterTank::CanInteractWith_Implementation(const AActor* Target) const
{
    if(!IsValid(Target)) return false;

    // 水箱只能与种植床交互
    return Cast<APlantBed>(Target) != nullptr;
}

void AWaterTank::OnInteractWith_Implementation(AActor* Target)
{
   TObjectPtr<APlantBed> PlantBed = Cast<APlantBed>(Target);
    if(!IsValid(PlantBed)) return;

    // 检查是否有足够多的水
    if(!HasEnoughResources(EResourcesType::Water, MoistureAmount)) return;
    
    PlantBed->ReceiveMoisture(MoistureAmount);

    RemoveResources(EResourcesType::Water, MoistureAmount);

    UE_LOG(A_LogAshFarm, Warning, TEXT("WaterTank: %s 添加了 %f 水"), *PlantBed->GetName(), MoistureAmount);
}

FString AWaterTank::GetAsSelectedHint_Implementation() const
{
    return TEXT("浇水");
}