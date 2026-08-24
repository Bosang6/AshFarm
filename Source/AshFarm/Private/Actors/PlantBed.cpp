// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlantBed.h"
#include "Components/BoxComponent.h"
#include "Comps/GreenHouseComponent.h"
#include "Comps/ProgressBarComponent.h"
#include "DrawDebugHelpers.h"
#include "AshFarm.h"
#include "Plants/PlantBase.h"
#include "Kismet/KismetSystemLibrary.h"

int32 APlantBed::TotalCount = 0;

// Sets default values
APlantBed::APlantBed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 降低Tick频率
	PrimaryActorTick.TickInterval = 0.5f;

	PlantingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("种植点"));
	PlantingPoint->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	PlantingPoint->SetRelativeLocation(FVector{0.0f, 0.0f, 20.0f});

	PlantMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlantMesh"));
	PlantMesh->AttachToComponent(PlantingPoint, FAttachmentTransformRules::KeepRelativeTransform);
	PlantMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProgressBarComponent = CreateDefaultSubobject<UProgressBarComponent>(TEXT("进度条组件"));
}

void APlantBed::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateSoilQuality();
	UpdatePlantMesh();   //初始化植物网格体
}

// Called when the game starts or when spawned
void APlantBed::BeginPlay()
{
	Super::BeginPlay();

	ensureAlwaysMsgf(MaxSoilFertility > 0.0f, TEXT("MaxSoilFertility 必须大于 0.0f"));
	ensureAlwaysMsgf(MaxMoisture >= 0.0f, TEXT("MaxMoisture 必须大于 0.0f"));

	// 初始化土壤肥力
	UpdateSoilQuality();

	TransitionCount = 0;  // 初始化土壤状态变换
	APlantBed::TotalCount++;

	InitNeighborBeds();

	// 绑定新植物的委托
	if(IsValid(CurrentPlant))
	{
		CurrentPlant->OnPlantMatured.AddDynamic(this, &APlantBed::OnPlantMatured);
	}
}

// Called every frame
void APlantBed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetFertilityLossPerSecond(DeltaTime);

	// 无辐射时，土壤肥力自愈
	if(RadiationLevel == 0)
	{
		SoilFertility += PlantBedDefaults::FERTILITY_RECOVER_PER_SECOND * DeltaTime;
		SoilFertility = FMath::Clamp(SoilFertility, 0.0f, MaxSoilFertility);
	}

	// 土壤水分流失
	SetMoistureLossPerSecond(DeltaTime);

	UpdateSoilQuality();

	// 植物生长
	if(CurrentPlant != nullptr)
	{
		// 构建生长环境上下文
		FPlantGrowthContext Context;
		Context.SoilType = SoilType;
		Context.SoilQuality = SoilQuality;
		Context.Fertility = SoilFertility;
		Context.Moisture = Moisture;
		Context.Temperature = Temperature;
		Context.RadiationLevel = RadiationLevel;
		Context.Toxicity = Toxicity;

		// 植物生长
		CurrentPlant->Grow(DeltaTime, Context);

		// 更新进度条组件
		if(IsValid(ProgressBarComponent))
		{
			ProgressBarComponent->SetProgress(CurrentPlant->GetGrowthProgressPercentage());
		}

		// 土壤水分消耗
		if(Moisture >= 0.0f)
		{
			Moisture -= CurrentPlant->PlantConfig.WaterConsumption * DeltaTime;
			Moisture = FMath::Clamp(Moisture, 0.0f, MaxMoisture);
		}

		// 肥耗
		if(SoilFertility >= 0.0f)
		{
			SoilFertility -= CurrentPlant->PlantConfig.FertilityConsumption * DeltaTime;
			SoilFertility = FMath::Clamp(SoilFertility, 0.0f, MaxSoilFertility);
		}

		UpdatePlantMesh();
	}

}

void APlantBed::EndPlay(const EEndPlayReason::Type EndplayReason)
{
	Super::EndPlay(EndplayReason);

	APlantBed::TotalCount--;
}

// 获取土壤肥力
float APlantBed::GetSoilFertility() const
{
	return SoilFertility;
}

// 设置土壤肥力
void APlantBed::SetSoilFertility(float Fertility)
{
	SoilFertility = Fertility;
}

// 获取土壤湿度
float APlantBed::GetSoilMoisture() const
{
	return Moisture;
}

// 获取土壤辐射等级
int32 APlantBed::GetRadiationLevel() const
{
	return RadiationLevel;
}

// 获取土壤毒性
float APlantBed::GetToxcity() const
{
	return Toxicity;
}

// 浇水
void APlantBed::ReceiveMoisture(float InMoisture)
{
	Moisture += InMoisture;
	Moisture = FMath::Clamp(Moisture, 0.0f, MaxMoisture);
}

// 施肥
void APlantBed::ReceiveFertility(float InFertility)
{
	SoilFertility += InFertility;
	SoilFertility = FMath::Clamp(SoilFertility, 0.0f, MaxSoilFertility);
}

// 获取所有种植床实例的数量
int32 APlantBed::GetTotalCount()
{
	return APlantBed::TotalCount;
}

// 更新土壤肥力
void APlantBed::UpdateSoilQuality()
{
	ESoilQuality NewQuality;

	// 如果土壤肥力低于盐碱地阈值, 土壤状态为盐碱地
	if(SoilFertility <= PlantBedDefaults::FERTILITY_SALINE_THRESHOLD)
	{
		NewQuality = ESoilQuality::Saline;
	}
	// 如果土壤肥力低于阈值, 土壤状态为贫瘠
	else if(SoilFertility < PlantBedDefaults::FERTILITY_POOR_THRESHOLD)
	{
		NewQuality = ESoilQuality::Poor;
	}
	// 如果土壤肥力低于高阈值、高于低阈值, 土壤状态为普通
	else if(SoilFertility < PlantBedDefaults::FERTILITY_FERTILE_THRESHOLD)
	{
		NewQuality = ESoilQuality::Normal;
	}
	// 如果土壤肥力高于高阈值, 土壤状态为肥沃
	else
	{
		NewQuality = ESoilQuality::Fertile;
	}

	if(NewQuality != SoilQuality)
	{
		TransitionCount++;
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d, 土壤状态: %s -> %s [第%d次转换]"), BedID,*GetSoilQualityText(SoilQuality), *GetSoilQualityText(NewQuality), TransitionCount);

		SoilQuality = NewQuality;

		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d 土壤状态： %s, 土壤肥力: %f"), BedID, *GetSoilQualityText(SoilQuality), SoilFertility);
	}
}

void APlantBed::UpdatePlantMesh()
{
	// 设置植物网格体
	if(CurrentPlant != nullptr)
	{
		CurrentPlant->SetGrowthStage();

		UStaticMesh* StageMesh = CurrentPlant->GetStageMesh();
		if(StageMesh != nullptr && PlantMesh->GetStaticMesh() != StageMesh)
		{
			PlantMesh->SetStaticMesh(StageMesh);
		}	
	}
	else
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("清除网格体"));
		PlantMesh->SetStaticMesh(nullptr);
	}

}

// 获取生长速度
float APlantBed::GetGrowthSpeed() const
{
	switch(SoilQuality)
	{
		case ESoilQuality::Poor:
			return GROWTH_SPEED_POOR;
		case ESoilQuality::Normal:
			return GROWTH_SPEED_NORMAL;
		case ESoilQuality::Fertile:
			return GROWTH_SPEED_FERTILE;
		case ESoilQuality::Saline:
			return GROWTH_SPEED_SALINE;

		default:
			UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d 未知土壤状态"), BedID);
			return GROWTH_SPEED_NORMAL;
	}
}

//课间挑战：获取枚举中文
FString APlantBed::GetSoilQualityText(ESoilQuality Quality) const
{
	switch(Quality)
	{
		case ESoilQuality::Poor:
			return TEXT("贫瘠");
		case ESoilQuality::Normal:
			return TEXT("正常");
		case ESoilQuality::Fertile:
			return TEXT("肥沃");
		case ESoilQuality::Saline:
			return TEXT("盐碱地");

		default:
			return TEXT("未知");
	}
}

FString APlantBed::GetSoilTypeText(ESoilType InSoilType) const
{
	switch(InSoilType)
	{
		case ESoilType::Sand:
			return TEXT("沙土");
		case ESoilType::Loam:
			return TEXT("泥土");
		case ESoilType::Clay:
			return TEXT("黏土");

		default:
			return TEXT("未知");
	}
}

// 更新土壤水分
void APlantBed::SetMoistureLossPerSecond(float DeltaTime)
{
	float GreenHouseLossReductionRate = 1.0f;

	if(TObjectPtr<UGreenHouseComponent> Comp = FindComponentByClass<UGreenHouseComponent>())
	{
		if(Comp->bEnabled)
		{
			GreenHouseLossReductionRate = Comp->MoistureLossReductionRate; 
		}
	}

	Moisture -= 
		PlantBedDefaults::MOISTURE_LOSS_PER_SECOND * 
		GetMoistureLossRateBySoilQuality() * 
		GetMoistureLossRateBySoilType() * GreenHouseLossReductionRate * 
		DeltaTime;
	Moisture = FMath::Clamp(Moisture, 0.0f, MaxMoisture);
}

// 土壤损失肥力
void APlantBed::SetFertilityLossPerSecond(float DeltaTime)
{
	SoilFertility -= 
		RadiationLevel * 
		PlantBedDefaults::FERTILITY_LOSS_PER_RADIATION * DeltaTime  								   // 辐射等级乘数
		+ PlantBedDefaults::FERTILITY_LOSS_PER_SECOND * GetFertilityLossRateBySoilType() * DeltaTime;  // 自然流失肥力
	SoilFertility = FMath::Clamp(SoilFertility, 0.0f, MaxSoilFertility);
}

// 根据土壤品质获取土壤湿度流失率
float APlantBed::GetMoistureLossRateBySoilQuality() const
{
	switch(SoilQuality)
	{
		case ESoilQuality::Poor:
			return PlantBedDefaults::MOISTURE_POOR_LOSS_MULTI;
		case ESoilQuality::Normal:
			return PlantBedDefaults::MOISTURE_NORMAL_LOSS_MULTI;
		case ESoilQuality::Fertile:
			return PlantBedDefaults::MOISTURE_FERTILE_LOSS_MULTI;
		case ESoilQuality::Saline:
			return PlantBedDefaults::MOISTURE_SALINE_LOSS_MULTI;

		default:
			UE_LOG(A_LogAshFarm, Warning, TEXT("种植床ID: %d 未知土壤状态"), BedID);
			return 0.0f;
	}
}

// 根据土壤类型获取土壤肥力流失率
float APlantBed::GetFertilityLossRateBySoilType() const
{
	switch(SoilType)
	{
		case ESoilType::Sand:
			return PlantBedDefaults::FERTILITY_SAND_LOSS_MULTI;
		case ESoilType::Loam:
			return PlantBedDefaults::FERTILITY_LOAM_LOSS_MULTI;
		case ESoilType::Clay:
			return PlantBedDefaults::FERTILITY_CLAY_LOSS_MULTI;

		default:
			return 1.0f;
	}
}

// 根据土壤类型获取土壤湿度流失率
float APlantBed::GetMoistureLossRateBySoilType() const
{
	switch(SoilType)
	{
		case ESoilType::Sand:
			return PlantBedDefaults::MOISTURE_SAND_LOSS_MULTI;
		case ESoilType::Loam:
			return PlantBedDefaults::MOISTURE_LOAM_LOSS_MULTI;
		case ESoilType::Clay:
			return PlantBedDefaults::MOISTURE_CLAY_LOSS_MULTI;

		default:
			return 1.0f;
	}
}

// 清除死亡植物
void APlantBed::ClearDeadPlant()
{
	if(!CurrentPlant)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("植物床ID: %d 没有植物"), BedID);
		return;
	}

	if(!CurrentPlant->IsDead())
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("植物床ID: %d 植物仍然存活"), BedID);
		return;
	}


	UE_LOG(A_LogAshFarm, Warning, TEXT("植物床ID: %d 清除死亡植物"), BedID);
	CurrentPlant = nullptr;

	UpdatePlantMesh();
	
}

// 收获植物
void APlantBed::Harvest()
{
	if(!CurrentPlant)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床: %d 没有植物"), BedID);
		return;
	}

	if(CurrentPlant->IsDead())
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床: %d 当前植物已死亡"), BedID);
		return;
	}

	if(!CurrentPlant->IsMature())
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床: %d 当前植物未成熟"), BedID);
		return;
	}

	UE_LOG(A_LogAshFarm, Warning, TEXT("种植床: %d 收获成熟植物"), BedID);

	FString PlantName = CurrentPlant->GetPlantName();
	FString HarvestName = CurrentPlant->PlantConfig.HarvestName.ToString();
	FString PlantQualityText = CurrentPlant->GetQualityText(CurrentPlant->CurrentQuality);
	float HarvestAmount = CurrentPlant->CalculateHarvest();

	// 计算附产品数量
	FString ByproductName = CurrentPlant->PlantConfig.ByproductName.ToString();
	float ByproductChance = CurrentPlant->PlantConfig.ByproductChance;
	float ByproductAmount = CurrentPlant->PlantConfig.ByproductAmount;

	// 产生一个随机数 0-1
	if(FMath::FRand() <= ByproductChance)
	{
		HarvestAmount += ByproductAmount;
	}

	UE_LOG(A_LogAshFarm, Warning, 
		TEXT("种植床: %d 收获成熟植物, 植物名称: %s, 收获物名称: %s 品质: %s, 收获量: %f \n\n \
				副产品名称: %s, 数量: %f"), 
				BedID,
				*PlantName, 
				*HarvestName, 
				*PlantQualityText, 
				HarvestAmount,
				*ByproductName,
				ByproductAmount
			);

	CurrentPlant = nullptr;
	UpdatePlantMesh();
}

// 打印状态
void APlantBed::PrintState()
{
	if(!IsValid(CurrentPlant))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床: %d 没有种植植物"), BedID);
		return;
	}

	FString PrintString = FString::Printf(TEXT("种植床ID: %d \n\n"), BedID);
	PrintString += FString::Printf(TEXT("土壤类型: %s, 土壤肥力: %f, 土壤品质: %s, 土壤湿度: %f \n\n"), *GetSoilTypeText(SoilType), 
			SoilFertility, 
			*GetSoilQualityText(SoilQuality), 
			Moisture);

	PrintString += FString::Printf(TEXT("目前作物: %s, 生长进度: %s (%f %s) - %s, 逆境值: %2f, 品质: %s \n\n"), *CurrentPlant->GetPlantName(),
			*CurrentPlant->GetGrowthStageText(CurrentPlant->GetGrowthStage()),
			CurrentPlant->GrowthProgress,
			*CurrentPlant->GetGrowthTimeText(),
			CurrentPlant->IsDead() ? TEXT("已死亡") : TEXT("正常"),
			CurrentPlant->Stress,
			*CurrentPlant->GetQualityText(CurrentPlant->CurrentQuality));
	PrintString += FString::Printf(TEXT("预计产量: %f \n\n"), CurrentPlant->CalculateHarvest());

	PrintString += FString::Printf(TEXT("相邻种植床数量: %d "), NeighborBeds.Num());

	for(const auto& NeighborBed : NeighborBeds)
	{
		PrintString += FString::Printf(TEXT("(%d)"), NeighborBed->BedID);
	}

	FVector TextLocation = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f); // 土壤肥力文本位置

	//DEBUG 打印信息
	DrawDebugString(
		GetWorld(), 
		TextLocation, 
		PrintString, 
		nullptr, 
		CurrentPlant->IsDead() ? FColor::Red : FColor::White, 
		1.5f); 
}

// 初始化相邻种植床
void APlantBed::InitNeighborBeds()
{
	if(!GetWorld())
	{
		return;
	}

	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = GetActorLocation();

	TArray<FHitResult> HitResults;

	bool bHit = UKismetSystemLibrary::BoxTraceMulti(
		GetWorld(),
		TraceStart,
		TraceEnd,
		FVector(NeighborDetectDistance, NeighborDetectDistance, 50.0f), 	// 盒体一半大小
		GetActorRotation(),													// Box朝向
		ETraceTypeQuery::TraceTypeQuery1,									// 物体类型查询
		false, 																// bTraceComplex
		TArray<AActor*>{this}, 												// Actors to Ignore
		EDrawDebugTrace::ForDuration,										// DrawDebugType
		HitResults,
		true,																// 忽略自己
		FColor::Red,
		FColor::Green,
		1.0f
	);

	if(!bHit)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("种植床: %d 没有找相邻的种植床"), BedID);
		return;
	}

	for(const auto& Hit : HitResults)
	{
		if(TObjectPtr<APlantBed> NeighborActor = Cast<APlantBed>(Hit.GetActor()))
		{
			if(NeighborActor == this)
			{
				continue;
			}

			NeighborBeds.Add(NeighborActor);
		}
	}
}

// ============== 
// 接口实现 
// ==============

// 选中时
void APlantBed::OnSelected_Implementation()
{
	Super::OnSelected_Implementation();
	PrintState();
}

// 交互时
void APlantBed::OnInteract_Implementation()
{
	Super::OnInteract_Implementation();
	Harvest();
}

// ==============
// 回调函数
// ==============

// 植物成熟时调用
void APlantBed::OnPlantMatured(UPlantBase* Plant)
{
	UE_LOG(A_LogAshFarm, Warning, TEXT("种植床: %d 植物 %s 已成熟"), BedID, *Plant->GetPlantName());
}