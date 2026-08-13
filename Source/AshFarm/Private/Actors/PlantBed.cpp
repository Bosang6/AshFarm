// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PlantBed.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "AshFarm.h"
#include "Plants/PlantBase.h"

int32 APlantBed::TotalCount = 0;

// Sets default values
APlantBed::APlantBed()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 降低Tick频率
	PrimaryActorTick.TickInterval = 0.5f;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if(MeshAsset.Succeeded())
	{
		Mesh->SetStaticMesh(MeshAsset.Object.Get());
	}
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	PlantingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("种植点"));
	PlantingPoint->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	PlantingPoint->SetRelativeLocation(FVector{0.0f, 0.0f, 20.0f});

	PlantMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlantMesh"));
	PlantMesh->AttachToComponent(PlantingPoint, FAttachmentTransformRules::KeepRelativeTransform);
	PlantMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("碰撞盒"));
	CollisionBox->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionBox->SetBoxExtent(FVector{100.0f, 100.0f, 100.0f});
	CollisionBox->SetRelativeLocation(FVector{0.0f, 0.0f, 20.0f});
	CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
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


		CurrentPlant->Grow(DeltaTime, Context);

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

	FVector TextLocation = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f); // 土壤肥力文本位置
	
	// DEBUG 打印信息
	DrawDebugString(
		GetWorld(), 
		TextLocation, 
		FString::Printf(TEXT("种植床ID: %d \n 土壤类型: %s, 土壤肥力: %f, 土壤品质: %s, 土壤湿度: %f \n 目前作物: %s, 生长进度: %s (%f), 逆境值: %2f, 品质: %s"), 
			BedID,
			*GetSoilTypeText(SoilType), 
			SoilFertility, 
			*GetSoilQualityText(SoilQuality), 
			Moisture,
			*CurrentPlant->GetPlantName(),
			*CurrentPlant->GetGrowthStageText(CurrentPlant->GetGrowthStage()),
			CurrentPlant->GrowthProgress,
			CurrentPlant->Stress,
			*CurrentPlant->GetQualityText(CurrentPlant->CurrentQuality)
		), 
		nullptr, 
		FColor::White, 
		DeltaTime);
	// Print Log
	//UE_LOG(LogTemp, Warning, TEXT("种植床数量: %d"), APlantBed::TotalCount); 
}

void APlantBed::EndPlay(const EEndPlayReason::Type EndplayReason)
{
	Super::EndPlay(EndplayReason);

	APlantBed::TotalCount--;
}

// 获取土壤肥力
float APlantBed::GetSoliFertility() const
{
	return SoilFertility;
}

// 设置土壤肥力
void APlantBed::SetSoliFertility(float Fertility)
{
	SoilFertility = Fertility;
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
		UE_LOG(A_LogAshFarm, Warning, TEXT("CurrentPlant没有被实例化!"));
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
	Moisture -= 
		PlantBedDefaults::MOISTURE_LOSS_PER_SECOND * 
		GetMoistureLossRateBySoilQuality() * 
		GetMoistureLossRateBySoilType() * DeltaTime;
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