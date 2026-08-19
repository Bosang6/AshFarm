#pragma once

#include "CoreMinimal.h"

#include "InventoryTypes.generated.h"

UENUM(BlueprintType)
enum class EResourcesType : uint8
{
    Water   UMETA(DisplayName = "水"),
    Wood    UMETA(DisplayName = "木材"),
    Soil    UMETA(DisplayName = "泥土"),
    Waste   UMETA(DisplayName = "废料"),
    Gold    UMETA(DisplayName = "金币"),
    Tool    UMETA(DisplayName = "工具"),
    Plant   UMETA(DisplayName = "种子"),
};

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


