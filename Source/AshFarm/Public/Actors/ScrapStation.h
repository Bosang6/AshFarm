// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Building.h"
#include "ScrapStation.generated.h"

/**
 * 
 */
UCLASS()
class ASHFARM_API AScrapStation : public ABuilding
{
	GENERATED_BODY()

public:
	// ========== 接口实现 ==============
	virtual void OnSelected_Implementation() override;
	virtual void OnInteract_Implementation() override;
	
};
