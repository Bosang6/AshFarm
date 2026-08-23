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

// 获取特效
UNiagaraSystem* UFeedbackDataAsset::GetEffect(FName EventName) const
{
	const TSoftObjectPtr<UNiagaraSystem> EffectPtr = EffectMap.FindRef(EventName);
	if(!EffectPtr.IsValid())
	{
		return nullptr;
	}

	return EffectPtr.LoadSynchronous();
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
	if(!FeedbackConfig)
	{
		FeedbackConfig = LoadObject<UFeedbackDataAsset>(this, TEXT("/Game/0_/Config/DA_FeedbackConfig.DA_FeedbackConfig"));
		if(FeedbackConfig)
		{
			UE_LOG(A_LogAshFarm, Warning, TEXT("成功加载音效数据表"));
		}
		else
		{
			UE_LOG(A_LogAshFarm, Error, TEXT("未能加载音效数据表，找不到资源: /Game/0_/Config/DA_FeedbackConfig.DA_FeedbackConfig"));
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
	if(!FeedbackConfig)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到反馈配置数据表"));
		return;
	}

	// 1. 先查缓存
	TObjectPtr<USoundBase> Sound = LoadedSoundMap.FindRef(EventName);
	if(!Sound)
	{
		// 2. 缓存没有，从配置数据资产中获取
		Sound = FeedbackConfig->GetSound(EventName);
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
	if(!FeedbackConfig)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到反馈配置数据表"));
		return;
	}

	// 1. 先查缓存
	TObjectPtr<USoundBase> Sound = LoadedSoundMap.FindRef(EventName);
	if(!Sound)
	{
		// 2. 缓存没有，从配置数据资产中获取
		Sound = FeedbackConfig->GetSound(EventName);
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

// 在指定位置处播放特效
void UFeedbackComponent::SpawnEffect(FName EventName, FVector Location, FRotator Rotation)
{
	if(!FeedbackConfig)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到反馈配置数据表"));
		return;
	}

	if(!FeedbackConfig->HasEffect(EventName))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到特效: %s"), *EventName.ToString());
		return;
	}

	TObjectPtr<UNiagaraSystem> Effect = FeedbackConfig->GetEffect(EventName);

	if(!Effect)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到特效: %s"), *EventName.ToString());
		return;
	}

	UNiagaraComponent* SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		Owner->GetWorld(),
		Effect,
		Location,
		Rotation,
		FVector(ScaleMultiplier),
		true, 						// 是否自动销毁
		true, 						// 是否自动激活
		ENCPoolMethod::AutoRelease  // 使用对象池
	);

	if(SpawnedComp)
	{
		ActiveEffectMap.FindOrAdd(EventName).Add(SpawnedComp);
	}
}

// 在指定位置处播放特效 (附着在指定组件上)
void UFeedbackComponent::SpawnEffectAttached(FName EventName, USceneComponent* AttachTo, FName SocketName)
{
	if(!FeedbackConfig)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到反馈配置数据表"));
		return;
	}

	if(!AttachTo)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未指定特效的挂载组件"));
		return;
	}

	if(!FeedbackConfig->HasEffect(EventName))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到特效: %s"), *EventName.ToString());
		return;
	}

	TObjectPtr<UNiagaraSystem> Effect = FeedbackConfig->GetEffect(EventName);

	if(!Effect)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("未找到特效: %s"), *EventName.ToString());
		return;
	}

	UNiagaraComponent* SpawnedComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		Effect,
		AttachTo,
		SocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true, 						// 是否自动销毁
		true, 						// 是否自动激活
		ENCPoolMethod::AutoRelease  // 使用对象池
	);

	if(SpawnedComp)
	{
		ActiveEffectMap.FindOrAdd(EventName).Add(SpawnedComp);
	}
}

// 停止播放特效
void UFeedbackComponent::StopEffect(FName EventName)
{
	TArray<TObjectPtr<UNiagaraComponent>> ActiveEffects = ActiveEffectMap.FindRef(EventName);

	if(ActiveEffects.IsEmpty()) return;

	for(UNiagaraComponent* Effect : ActiveEffects)
	{
		Effect->Deactivate();
	}

	ActiveEffects.Empty();
}