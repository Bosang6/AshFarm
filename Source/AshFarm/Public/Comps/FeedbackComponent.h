// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"			// 生存Niagara的核心工具库
#include "NiagaraComponent.h"				// Niagara 组件本身
#include "FeedbackComponent.generated.h"

#pragma region 反馈配置数据资产

UCLASS(BlueprintType, meta = (DisplayName = "反馈配置数据资产"))
class ASHFARM_API UFeedbackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "反馈配置", meta = (DisplayName = "音效映射表"))
	TMap<FName, TSoftObjectPtr<USoundBase>> SoundMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "反馈配置", meta = (DisplayName = "特效映射表"))
	TMap<FName, TSoftObjectPtr<UNiagaraSystem>> EffectMap;

	// 获取音效 (加载资源) 【UFUNCTION 下 不能返回 TObjectPtr】
	UFUNCTION(BlueprintCallable, Category = "反馈配置")
	USoundBase* GetSound(FName EventName) const;

	// 获取特效
	UFUNCTION(BlueprintCallable, Category = "反馈配置")
	UNiagaraSystem* GetEffect(FName EventName) const;

	// 检查音效是否存在
	UFUNCTION(BlueprintCallable, Category = "反馈配置")
	bool HasSound(FName EventName) const { return SoundMap.Contains(EventName); };

	// 检查特效是否存在
	UFUNCTION(BlueprintCallable, Category = "反馈配置")
	bool HasEffect(FName EventName) const { return EffectMap.Contains(EventName); };
};


#pragma endregion

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ASHFARM_API UFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFeedbackComponent();

	// ==================
	// 配置数据
	// ==================

	// 反馈配置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "音效配置", meta = (DisplayName = "反馈配置数据资产"))
	TObjectPtr<UFeedbackDataAsset> FeedbackConfig = nullptr;

	// 音量倍数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "音效配置", meta = (DisplayName = "音量乘数"))
	float VolumeMultiplier = 1.0f;

	// 音高倍数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "音效配置", meta = (DisplayName = "音高乘数"))
	float PitchMultiplier = 1.0f;

	// 特效缩放倍数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "特效配置", meta = (DisplayName = "特效缩放乘数"))
	float ScaleMultiplier = 1.0f;

	// ==================
	// 功能函数
	// ==================

	// ============ 音效 ================

	// 播放音效 2D
	UFUNCTION(BlueprintCallable, Category = "音效配置", meta = (DisplayName = "播放音效"))
	void PlaySound(FName EventName);

	// 在指定位置处播放音效
	UFUNCTION(BlueprintCallable, Category = "音效配置", meta = (DisplayName = "在指定位置处播放音效"))
	void PlaySoundAtLocation(FName EventName, FVector Location);

	// ============ 特效 ================

	// 在指定位置处播放特效
	UFUNCTION(BlueprintCallable, Category = "特效配置", meta = (DisplayName = "在指定位置处播放特效"))
	void SpawnEffect(FName EventName, FVector Location, FRotator Rotation);

	// 在指定位置处播放特效 (附着在指定组件上)
	UFUNCTION(BlueprintCallable, Category = "特效配置", meta = (DisplayName = "在指定位置处播放特效 (附着在指定组件上)"))
	void SpawnEffectAttached(FName EventName, USceneComponent* AttachTo, FName SocketName);

	// 停止播放特效
	UFUNCTION(BlueprintCallable, Category = "特效配置", meta = (DisplayName = "停止播放特效"))
	void StopEffect(FName EventName);

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
