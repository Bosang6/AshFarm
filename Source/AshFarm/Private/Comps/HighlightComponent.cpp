// Fill out your copyright notice in the Description page of Project Settings.


#include "Comps/HighlightComponent.h"
#include "AshFarm.h"

// Sets default values for this component's properties
UHighlightComponent::UHighlightComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHighlightComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if(!IsValid(GetOwner()))
	{
		UE_LOG(A_LogAshFarm, Warning, TEXT("冷却组件: Owner 无效"));
		return;
	}
	Owner = GetOwner();	
	
}


// Called every frame
void UHighlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// 设置高亮状态
void UHighlightComponent::SetHighlight(bool bHighlight)
{
	if(!IsValid(Owner)) return;

	TArray<TObjectPtr<UStaticMeshComponent>> Meshs;
	Owner->GetComponents<UStaticMeshComponent>(Meshs);

	if(Meshs.IsEmpty()) return;

	for(const auto& Mesh : Meshs)
	{
		// 设定自定义深度渲染
		Mesh->SetRenderCustomDepth(bHighlight);
		// 设定自定义深度模板值
		Mesh->SetCustomDepthStencilValue(bHighlight ? 3 : 0);
	}

}