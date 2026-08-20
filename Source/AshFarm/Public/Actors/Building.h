// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Interactable.h"

#include "Building.generated.h"

class UBoxComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class ASHFARM_API ABuilding : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 根组件
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Root")
	TObjectPtr<USceneComponent> Root;

	// 静态网格体组件
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (DisplayName = "植物床网格体"))
	TObjectPtr<UStaticMeshComponent> Mesh;

	// 碰撞盒组件
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Collision")
	TObjectPtr<UBoxComponent> CollisionBox;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ========== 接口实现 ==============
	virtual void OnSelected_Implementation() override;
	virtual void OnInteract_Implementation() override;
	// =================================

};
