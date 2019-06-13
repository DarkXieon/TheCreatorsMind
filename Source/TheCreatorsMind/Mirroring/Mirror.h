// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/PlanarReflection.h"
#include "Engine/SceneCapture2D.h"
#include "EnviromentMirrorer.h"
#include "ChildReflection.h"
#include "ChildCapture.h"
#include "Mirror.generated.h"

UCLASS()
class THECREATORSMIND_API AMirror : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	AMirror();

	uint8 bRunConstructionScriptOnDrag = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	int TextureId = 1;

	//UPROPERTY(VisibleAnywhere, Category = Test, meta = (ExposeFunctionCategories = "Static Mesh", AllowPrivateAccess = "true"))
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PlaneVisual;

	UPROPERTY(VisibleAnywhere)
	UEnviromentMirrorer* EnviromentMirrorer;

	UPROPERTY(EditInstanceOnly, Instanced, Category = Components)
	UChildReflection* ChildReflection;

	UPROPERTY(EditInstanceOnly, Instanced, Category = Components)
	UChildCapture* ChildCapture;

	//UPROPERTY(EditAnywhere, Category = Materials)
	TWeakObjectPtr<UMaterialInterface> MirrorMaterial; //UMaterialInterface* MirrorMaterial;

	//UPROPERTY(EditAnywhere, Category = Materials)
	TWeakObjectPtr<UMaterialInterface> TextureMaterial; //UMaterialInterface* TextureMaterial;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UStaticMeshComponent* GetPlaneVisualComponent() { return PlaneVisual; }
	UEnviromentMirrorer* GetEnviromentMirrorerComponent() { return EnviromentMirrorer; }

	APlanarReflection* GetPlanarReflectionActor() 
	{ 
		TArray<AActor*> Attached; 
		GetAttachedActors(Attached); 
		AActor** Found = nullptr;
		//UE_LOG(LogTemp, Fatal, TEXT("%d"), Attached.Num());
		Found = Attached.FindByPredicate([](AActor* actor) { return actor->IsA<APlanarReflection>(); });
		return Found ? (APlanarReflection*)*Found : nullptr;
	}

	ASceneCapture2D* GetSceneCapture2DActor() 
	{
		TArray<AActor*> Attached;
		GetAttachedActors(Attached);
		AActor** Found = nullptr;
		Found = Attached.FindByPredicate([](AActor* actor) { return actor->IsA<ASceneCapture2D>(); });
		return Found ? (ASceneCapture2D*)*Found : nullptr;
	}

	void UseMirrorMaterial() 
	{
		if (MirrorMaterial.IsValid() && PlaneVisual->GetMaterial(0) != MirrorMaterial.Get())
			PlaneVisual->SetMaterial(0, MirrorMaterial.Get()); 
	}

	void UseTextureMaterial() 
	{
		if(TextureMaterial.IsValid() && PlaneVisual->GetMaterial(0) != TextureMaterial.Get())
			PlaneVisual->SetMaterial(0, TextureMaterial.Get()); 
	}
};