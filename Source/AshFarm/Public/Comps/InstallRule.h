#pragma once

#include "CoreMinimal.h"

#include "Components/SceneComponent.h"

#include "InstallRule.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "功能组件安装规则"))
struct FInstallRule : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "功能组件", meta = (DisplayName = "功能组件类"))
    TSubclassOf<UActorComponent> ComponentClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "功能组件", meta = (DisplayName = "允许安装的宿主"))
    TSet<TSubclassOf<AActor>> ValidOwners;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "功能组件", meta = (DisplayName = "是否允许重复安装"))
    bool bAllowDuplicate;
};