// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/AutoWateringComponent.h"
#include "Inventory/WaterTank.h"
#include "Actors/PlantBed.h"
#include "AshFarm.h"
#include "EngineUtils.h" // 用于遍历World中所有的Actor => TActorIterator

// Sets default values for this component's properties
UAutoWateringComponent::UAutoWateringComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;

	// ...
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("自动浇水网格体"));
	Mesh->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	
	// 加载默认静态网格体 （临时先用一下，后期改）
	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/PolygonFarm/Meshes/Props/SM_Prop_LetterBox_01.SM_Prop_LetterBox_01"));
	if(!MeshAsset.Succeeded())
	{
		MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Engine/BasicShapes/Cube.Cube"));
	}
	Mesh->SetStaticMesh(MeshAsset.Object.Get());
	Mesh->SetRelativeLocation(FVector(50.0f, 50.0f, 0.0f));
}


// Called when the game starts
void UAutoWateringComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: 开始初始化"));

	// 获取 Owner
	if(!IsValid(GetOwner()))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: Owner无效, 无法继续进行初始"));
		return;
	}
	if(Owner = Cast<APlantBed>(GetOwner()))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: Owner: %s"), *Owner->GetName());
	}
	else
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: Owner 不是种植床"));
	}

	// 查找世界中的第一个 WaterTank
	if(!IsValid(WaterSource))
	{
		for(TActorIterator<AWaterTank> It(GetWorld()); It; ++It)
		{
			WaterSource = *It;
			break;
		}

		// 判断是否真正拿到水箱指针
		if(IsValid(WaterSource))
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: 找到水源水箱, %s"), *WaterSource->GetName());
		}

		else
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: 未找到水源水箱, 请确保场景中最少有一个WaterTank"));
		}
	}
	
}


// Called every frame
void UAutoWateringComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if(!bEnabled)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: 组件未启用，未执行浇水操作"));
		return;
	}

	if(!IsValid(WaterSource))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: 水源水箱未初始化，未执行浇水操作"));
		return;
	}

	if(!IsValid(Owner))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: Owner 未初始化"));
		return;
	}

	// 检查土壤湿度
	if(Owner->GetSoliMoisture() < MoistureThreshold)
	{
		// 检查水箱是否有水
		if(WaterSource->GetResourceCount(EResourcesType::Water) < WaterAmountPerTick)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: 水源水箱水量不足，未执行浇水操作"));
			return;
		}

		// 计算每次浇水量是否超过水箱余量
		float WaterToTake = FMath::Min(WaterAmountPerTick, WaterSource->GetResourceCount(EResourcesType::Water));
		// 从水箱中移除水
		// ===============================================================================================
		// 这里有BUG, WaterAmountPerTick = 0.5 作为RemoveResource() 的参数，在数据转换为int32时为0
		UE_LOG(A_LogAshFarm, Warning, TEXT("自动浇水组件: 浇水 %d"), WaterSource->RemoveResource(EResourcesType::Water, WaterToTake));
		WaterSource->RemoveResource(EResourcesType::Water, WaterToTake);
		// =====================================================================================================
		// 浇水
		Owner->ReceiveMoisture(WaterToTake);
	}

}

