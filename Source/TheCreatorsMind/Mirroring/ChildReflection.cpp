// Fill out your copyright notice in the Description page of Project Settings.

#include "ChildReflection.h"
#include "Engine/PlanarReflection.h"
#include "Components/PlanarReflectionComponent.h"

// Sets default values for this component's properties
UChildReflection::UChildReflection()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetChildActorClass(APlanarReflection::StaticClass());
	bEditableWhenInherited = true;
	// ...
}


// Called when the game starts
void UChildReflection::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UChildReflection::OnRegister()
{
	Super::OnRegister();

	FVector Location(0.0f, 0.0f, 0.5f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FVector Scale(0.025f, 0.025f, 1.0f);
	FTransform Transform(Rotation, Location, Scale);
	SetRelativeTransform(Transform);

	APlanarReflection* PlanarReflection = (APlanarReflection*)GetChildActor();
	PlanarReflectionComponent = PlanarReflection->GetPlanarReflectionComponent();
	PlanarReflectionComponent->ScreenPercentage = 100;
	PlanarReflectionComponent->PrefilterRoughness = 0.0f;
	PlanarReflectionComponent->DistanceFromPlaneFadeoutStart = 5.0f;
	PlanarReflectionComponent->DistanceFromPlaneFadeoutEnd = 5.0f;
	PlanarReflectionComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;


}


// Called every frame
void UChildReflection::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

