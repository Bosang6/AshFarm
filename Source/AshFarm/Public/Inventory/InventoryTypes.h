#pragma once

#include "CoreMinimal.h"

#include "InventoryTypes.generated.h"

UENUM(BlueprintType)
enum class EResourcesType : uint8
{
    Water       UMETA(DisplayName = "水"),
    Wood        UMETA(DisplayName = "木材"),
    Soil        UMETA(DisplayName = "泥土"),
    Fertility   UMETA(DisplayName = "肥料"),
    Waste       UMETA(DisplayName = "废料"),
    Gold        UMETA(DisplayName = "金币"),
    Tool        UMETA(DisplayName = "工具"),
    Plant       UMETA(DisplayName = "种子"),
};

// 排序类型
UENUM(BlueprintType)
enum class EInventorySortType : uint8
{
    TypeAscending      UMETA(DisplayName = "类型升序"),
    TypeDescending     UMETA(DisplayName = "类型降序"),
    CountAscending     UMETA(DisplayName = "数量升序"),
    CountDescending    UMETA(DisplayName = "数量升序"),
};

// 资源配置
USTRUCT(BlueprintType)
struct FResourcesConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源配置", meta = (DisplayName = "资源类型"))
    EResourcesType Type = EResourcesType::Water;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源配置", meta = (DisplayName = "显示名称"))
    FText DisplayName = FText::FromString(TEXT("水"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源配置", meta = (DisplayName = "详情"))
    FText Description = FText::FromString(TEXT("水是植物生长的基础, 用于植物的生长和生存"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源配置", meta = (DisplayName = "最大数量"))
    int32 MaxCapacity = 200;

    // 警戒阈值 (占比)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源配置", meta = (DisplayName = "警戒阈值 (占比)", ClampMin = "0.0", ClampMax = "1.0"))
    float LowThresholdPercent = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源配置", meta = (DisplayName = "图标"))
    TSoftObjectPtr<UTexture2D> Icon;
};


USTRUCT(BlueprintType)
struct FResourceBundle
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源包", meta = (DisplayName = "资源类型"))
    EResourcesType Type;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "资源包", meta = (DisplayName = "资源数量", ClampMin = "0"))
    int32 Count;
};


