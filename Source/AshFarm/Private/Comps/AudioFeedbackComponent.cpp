// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/AudioFeedbackComponent.h"

#pragma region 音效配置数据资产
// 获取音效 (加载资源)
USoundBase* UAudioFeedbackDataAsset::GetSound(FName EventName) const
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
UAudioFeedbackComponent::UAudioFeedbackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAudioFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAudioFeedbackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

