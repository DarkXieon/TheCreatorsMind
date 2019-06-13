// Fill out your copyright notice in the Description page of Project Settings.

#include "ChildCapture.h"
#include "Engine/SceneCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"

// Sets default values for this component's properties
UChildCapture::UChildCapture()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetChildActorClass(ASceneCapture2D::StaticClass());
	bEditableWhenInherited = true;
	bAllowReregistration = false;
	bReplicates = false;
	
	// ...
}


// Called when the game starts
void UChildCapture::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UChildCapture::OnRegister()
{
	Super::OnRegister();

	FVector Location(0.0f, 0.0f, 0.5f);
	FRotator Rotation(90.0f, 0.0f, -90.0f);
	FVector Scale(0.2f, 0.2f, 1.0f);
	FTransform Transform(Rotation, Location, Scale);
	SetRelativeTransform(Transform);

	//CaptureComponent2D = ((ASceneCapture2D*)GetChildActor())->GetCaptureComponent2D();

	if (RenderTexture)
	{
		ASceneCapture2D* CaptureActor2D = (ASceneCapture2D*)GetChildActor();
		USceneCaptureComponent2D* CaptureComponent2D = CaptureActor2D->GetCaptureComponent2D();
		CaptureComponent2D->TextureTarget = RenderTexture;

		CaptureComponent2D->bCaptureEveryFrame = true;
		CaptureComponent2D->bCaptureOnMovement = true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Registered"));
}

// Called every frame
void UChildCapture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

