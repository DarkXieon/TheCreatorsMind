// Fill out your copyright notice in the Description page of Project Settings.

#include "EnviromentMirrorer.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UEnviromentMirrorer::UEnviromentMirrorer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void UEnviromentMirrorer::BeginPlay()
{
	Super::BeginPlay();
	
	AActor* parent = UActorComponent::GetOwner();
	/*
	FScriptDelegate onOverlapDelegate;
	onOverlapDelegate.BindUFunction(this, "OnOverlapBegin");
	parent->OnActorBeginOverlap.Add(onOverlapDelegate);
	*/
}

// Called every frame
void UEnviromentMirrorer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
//
//void UEnviromentMirrorer::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Collision Occured"));
//}