// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/ChildActorComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ChildCapture.generated.h"


//UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
UCLASS()
class THECREATORSMIND_API UChildCapture : public UChildActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChildCapture();

protected:

	//UPROPERTY(EditInstanceOnly, Instanced, Category = Components)
	//USceneCaptureComponent2D* CaptureComponent2D;

	UPROPERTY(EditAnywhere, Category = Test)
	UTextureRenderTarget2D* RenderTexture;

	UPROPERTY(EditAnywhere, Category = Test)
	FString TexturePath;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnRegister() override;
};
