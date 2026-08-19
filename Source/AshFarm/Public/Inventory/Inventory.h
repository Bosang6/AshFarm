// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class ASHFARM_API AInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	AInventory();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Root")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (DisplayName = "植物床网格体"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 碰撞盒组件
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UBoxComponent> CollisionBox;

public:	
	virtual void Tick(float DeltaTime) override;

};
