// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/AutoFertilityComponent.h"
#include "Actors/PlantBed.h"
#include "AshFarm.h"
#include "EngineUtils.h" // 用于遍历World中所有的Actor => TActorIterator

// Sets default values for this component's properties
UAutoFertilityComponent::UAutoFertilityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;

	// ...
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("自动施肥网格体"));
	Mesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	
	// 加载默认静态网格体 （临时先用一下，后期改）
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/PolygonFarm/Meshes/Props/SM_Prop_Vice_Slide_01.SM_Prop_Vice_Slide_01"));
	if(!MeshAsset.Succeeded())
	{
		MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	Mesh->SetStaticMesh(MeshAsset.Object.Get());
	Mesh->SetRelativeLocation(FVector(50.0f, 50.0f, 0.0f));
}


// Called when the game starts
void UAutoFertilityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: 开始初始化"));

	// 获取 Owner
	if(!IsValid(GetOwner()))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: Owner无效, 无法继续进行初始"));
		return;
	}
	if(Owner = Cast<APlantBed>(GetOwner()))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: Owner: %s"), *Owner->GetName());
	}
	else
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: Owner 不是种植床"));
	}
}


// Called every frame
void UAutoFertilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(!bEnabled)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: 组件未启用，未执行施肥操作"));
		return;
	}

	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: Owner 未初始化"));
		return;
	}

	// 检查土壤湿度
	if(Owner->GetSoliFertility() < FertilityThreshold)
	{
		// 检查水箱是否有水
		if(InternalStorage <= 0)
		{
			if(!bLastFailed)
			{
				UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: 肥料不足，未执行施肥操作"));
				bLastFailed = true;
			}
			return;
		}

		// 计算每次施肥量是否超过余量
		float ToTake = FMath::Min(FertilityAmountPerTick, InternalStorage);
		// 从水箱中移除水
		InternalStorage -= ToTake;
		InternalStorage = FMath::Clamp(InternalStorage, 0, MaxStorage);
		// 施肥
		Owner->ReceiveFertility(ToTake);

		bLastFailed = false;
	}
	else
	{
		bLastFailed = false;
	}
}

// 重新填充
void UAutoFertilityComponent::Refill(int32 Amount)
{
	if(Amount <= 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: 重新填充量必须大于 0"));
		return;
	}

	// 记录旧值
	int32 LastStorage = InternalStorage;
	// 填充肥料
	InternalStorage = FMath::Clamp(InternalStorage + Amount, 0, MaxStorage);
	// 计算实际填充量
	int32 Added = InternalStorage - LastStorage;

	UE_LOG(A_LogAshFarm, Warning, TEXT("自动施肥组件: 重新填充量 %d, 实际填充量 %d, 当前储备 %d"), Amount, Added, InternalStorage);
}

