// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/StateTagComponent.h"
#include "AshFarm.h"

// Sets default values for this component's properties
UStateTagComponent::UStateTagComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStateTagComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStateTagComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 添加标签
void UStateTagComponent::AddTag(FName Tag)
{
	if(Tag == NAME_None)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("[状态标签组件] 添加的标签为空"));
		return;
	}

	if(ActiveTags.Contains(Tag))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("[状态标签组件] 标签 %s 已存在"), *Tag.ToString());
		return;
	}

	ActiveTags.Add(Tag);
	UE_LOG(A_LogAshFarm, Warning, TEXT("[状态标签组件] 标签 %s 添加成功"), *Tag.ToString());
}

// 移除标签
void UStateTagComponent::RemoveTag(FName Tag)
{
	if(Tag == NAME_None)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("[状态标签组件] 移除的标签为空"));
		return;
	}

	if(ActiveTags.Remove(Tag) > 0)
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("[状态标签组件] 标签 %s 移除成功"), *Tag.ToString());
	}
	else
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("[状态标签组件] 标签 %s 不存在"), *Tag.ToString());
	}
}

// 检查标签是否存在
bool UStateTagComponent::HasTag(FName Tag) const
{
	return ActiveTags.Contains(Tag);
}

// 清除所有标签
void UStateTagComponent::ClearAllTags()
{
	int32 Count = ActiveTags.Num();
	ActiveTags.Empty();
	UE_LOG(A_LogAshFarm, Warning, TEXT("[状态标签组件] 清除了 %d 个标签"), Count);
}

// 获取当前激活的标签字符串
FString UStateTagComponent::GetTagsAsString() const
{
	if(ActiveTags.IsEmpty())
	{
		return TEXT("无标签");
	}

	FString TagAsString = TEXT("");
	for(const FName& Tag : ActiveTags)
	{
		if(!ActiveTags.IsEmpty())
		{
			TagAsString += TEXT(" | ");
		}

		TagAsString += Tag.ToString();
	}

	return TagAsString;
}

// 获取标签的数量
int32 UStateTagComponent::GetTagsCount() const
{
	return ActiveTags.Num();
}