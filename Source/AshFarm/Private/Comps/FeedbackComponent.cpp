// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/FeedbackComponent.h"
#include "AshFarm.h"
#include "Kismet/GameplayStatics.h"

#pragma region 音效配置数据资产
// 获取音效 (加载资源)
USoundBase* UFeedbackDataAsset::GetSound(FName EventName) const
{
	// SoundMap.Find(EventName) 返回的是一个指向 TSoftObjectPtr<USoundBase> 的指针 【指向指针的指针】
	const TSoftObjectPtr<USoundBase> SoundPtr = SoundMap.FindRef(EventName);
	if(!SoundPtr.IsValid())  // 软对象指针的判合法方式与对象不同 【IsValid(SoundPtr)】
	{
		return nullptr;
	}

	return SoundPtr.LoadSynchronous(); // 软对象指针 并不是一个真正的指针，是一个对象。 这里需要用 . 进行函数调用
}

#pragma endregion


// Sets default values for this component's properties
UFeedbackComponent::UFeedbackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	if(!Owner)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("音效组件: Owner无效"));
		return;
	}

	// 加载数据资产
	if(!AudioConfig)
	{
		AudioConfig = LoadObject<UFeedbackDataAsset>(this, TEXT("/Game/0_/Config/DA_AudioConfig.DA_AudioConfig"));
		if(AudioConfig)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("成功加载音效数据表"));
		}
		else
		{
			UE_LOG(A_LogAshFarm, Error, TEXT("未能加载音效数据表，找不到资源: /Game/0_/Config/DA_AudioConfig.DA_AudioConfig"));
		}
	}

	
}


// Called every frame
void UFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 播放音效 2D
void UFeedbackComponent::PlaySound(FName EventName)
{
	if(!AudioConfig)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到音效配置数据表"));
		return;
	}

	// 1. 先查缓存
	TObjectPtr<USoundBase> Sound = LoadedSoundMap.FindRef(EventName);
	if(!Sound)
	{
		// 2. 缓存没有，从配置数据资产中获取
		Sound = AudioConfig->GetSound(EventName);
		if(!Sound)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("未找到音效: %s"), *EventName.ToString());
			return;
		}
		// 3.缓存音效
		LoadedSoundMap.Add(EventName, Sound);
	}

	UGameplayStatics::PlaySound2D(Owner->GetWorld(), Sound, VolumeMultiplier, PitchMultiplier);
}

// 在指定位置处播放音效
void UFeedbackComponent::PlaySoundAtLocation(FName EventName, FVector Location)
{
	if(!AudioConfig)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到音效配置数据表"));
		return;
	}

	// 1. 先查缓存
	TObjectPtr<USoundBase> Sound = LoadedSoundMap.FindRef(EventName);
	if(!Sound)
	{
		// 2. 缓存没有，从配置数据资产中获取
		Sound = AudioConfig->GetSound(EventName);
		if(!Sound)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("未找到音效: %s"), *EventName.ToString());
			return;
		}
		// 3.缓存音效
		LoadedSoundMap.Add(EventName, Sound);
	}

	UGameplayStatics::PlaySoundAtLocation(Owner->GetWorld(), Sound, Location, VolumeMultiplier, PitchMultiplier);

}