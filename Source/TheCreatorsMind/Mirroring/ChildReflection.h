// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "Components/PlanarReflectionComponent.h"
#include "ChildReflection.generated.h"


//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS()
class THECREATORSMIND_API UChildReflection : public UChildActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChildReflection();

protected:

	UPROPERTY(EditInstanceOnly, Instanced, Category = Components)
	UPlanarReflectionComponent* PlanarReflectionComponent;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;
};
