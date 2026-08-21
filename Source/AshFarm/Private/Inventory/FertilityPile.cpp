// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/FertilityPile.h"

#include "Actors/PlantBed.h"

bool AFertilityPile::CanInteractWith_Implementation(const AActor* Target) const
{
    if(!IsValid(Target)) return false;

    // 肥料堆只能与种植床交互
    return Cast<APlantBed>(Target) != nullptr;
}

void AFertilityPile::OnInteractWith_Implementation(AActor* Target)
{
   TObjectPtr<APlantBed> PlantBed = Cast<APlantBed>(Target);
    if(!IsValid(PlantBed)) return;

    // 检查是否有足够多的肥料
    if(!HasEnoughResources(EResourcesType::Fertility, FertilityAmount)) return;
    
    PlantBed->ReceiveFertility(FertilityAmount);

    RemoveResource(EResourcesType::Fertility, FertilityAmount);

    UE_LOG(A_LogAshFarm, Warning, TEXT("FertilityPile: %s 施了 %f 肥"), *PlantBed->GetName(), FertilityAmount);
}

FString AFertilityPile::GetAsSelectedHint_Implementation() const
{
    return TEXT("施肥");
}