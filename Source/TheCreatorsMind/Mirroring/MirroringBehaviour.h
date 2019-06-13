// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MirroringBehaviour.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class THECREATORSMIND_API UMirroringBehaviour : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMirroringBehaviour();

protected:

	TMap<UPrimitiveComponent*, FCollisionResponseContainer> DisplayedCollision;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

	void WorldHide();

	void WorldShow();

	void ReverseCulling();

	void Initialize();
};
