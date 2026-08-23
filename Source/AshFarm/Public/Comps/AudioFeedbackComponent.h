// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/ActorComponent.h"
#include "AudioFeedbackComponent.generated.h"

#pragma region 音效配置数据资产

UCLASS(BlueprintType, meta = (DisplayName = "音效配置数据资产"))
class ASHFARM_API UAudioFeedbackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "音效配置", meta = (DisplayName = "音效映射表"))
	TMap<FName, TSoftObjectPtr<USoundBase>> SoundMap;

	// 获取音效 (加载资源) 【UFUNCTION 下 不能返回 TObjectPtr】
	UFUNCTION(BlueprintCallable, Category = "音效配置")
	USoundBase* GetSound(FName EventName) const;

	// 检查音效是否存在
	UFUNCTION(BlueprintCallable, Category = "音效配置")
	bool HasSound(FName EventName) const { return SoundMap.Contains(EventName); };
};


#pragma endregion

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UAudioFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAudioFeedbackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
