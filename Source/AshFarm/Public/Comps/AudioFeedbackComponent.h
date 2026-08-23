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

	// ==================
	// 配置数据
	// ==================

	// 音效配置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "音效配置", meta = (DisplayName = "音效配置数据资产"))
	TObjectPtr<UAudioFeedbackDataAsset> AudioConfig = nullptr;

	// 音量倍数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "音效配置", meta = (DisplayName = "音量乘数"))
	float VolumeMultiplier = 1.0f;

	// 音高倍数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "音效配置", meta = (DisplayName = "音高乘数"))
	float PitchMultiplier = 1.0f;

	// ==================
	// 功能函数
	// ==================

	// 播放音效 2D
	UFUNCTION(BlueprintCallable, Category = "音效配置", meta = (DisplayName = "播放音效"))
	void PlaySound(FName EventName);

	// 在指定位置处播放音效
	UFUNCTION(BlueprintCallable, Category = "音效配置", meta = (DisplayName = "在指定位置处播放音效"))
	void PlaySoundAtLocation(FName EventName, FVector Location);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TObjectPtr<AActor> Owner = nullptr;

	// 已加载的音效映射表
	TMap<FName, TObjectPtr<USoundBase>> LoadedSoundMap;
};
