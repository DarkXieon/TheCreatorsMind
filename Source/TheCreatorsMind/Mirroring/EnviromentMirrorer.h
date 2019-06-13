// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnviromentMirrorer.generated.h"


UCLASS( ClassGroup=(EnviromentMirrorer), meta=(BlueprintSpawnableComponent) )
class THECREATORSMIND_API UEnviromentMirrorer : public UActorComponent
{
	GENERATED_BODY()
public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mirror")
	bool TestValue = false;

	// Sets default values for this component's properties
	UEnviromentMirrorer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
/*
private:

	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);*/
};
