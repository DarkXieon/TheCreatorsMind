// Fill out your copyright notice in the Description page of Project Settings.

#include "Mirror.h"
#include "Components/PlanarReflectionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SceneComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/ConstructorHelpers.h"
#include "EnviromentMirrorer.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"

// Sets default values
AMirror::AMirror()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UStaticMeshComponent* PlaneComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mirror Surface"));
	RootComponent = PlaneComponent;
	PlaneVisual = PlaneComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneVisualAsset(TEXT("/Engine/BasicShapes/Plane"));
	if (PlaneVisualAsset.Succeeded())
	{
		PlaneVisual->SetStaticMesh(PlaneVisualAsset.Object);

		static ConstructorHelpers::FObjectFinder<UMaterialInterface> MirrorMaterialInterface(TEXT("MaterialInstanceConstant'/Game/Materials/MirrorMaterials/Mirror'"));
		if (MirrorMaterialInterface.Succeeded())
		{
			PlaneVisual->SetMaterial(0, MirrorMaterialInterface.Object);
			MirrorMaterial = MirrorMaterialInterface.Object;
		}

		const FString materialName = FString::Printf(TEXT("MaterialInstanceConstant'/Game/Materials/MirrorMaterials/InstancedTextureMaterials/MirrorTextureMaterial_%d'"), TextureId);
		static ConstructorHelpers::FObjectFinder<UMaterialInstance> TextureMaterialInterface(*materialName);
		if (TextureMaterialInterface.Succeeded())
		{
			TextureMaterial = TextureMaterialInterface.Object;
		}

		PlaneVisual->SetMobility(EComponentMobility::Static);
		PlaneVisual->OnComponentBeginOverlap.AddDynamic(this, &AMirror::OnOverlapBegin);

		FCollisionResponseContainer ResponseContainer(ECollisionResponse::ECR_Block);
		PlaneVisual->SetGenerateOverlapEvents(true);
		PlaneVisual->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		PlaneVisual->SetCollisionResponseToChannels(ResponseContainer);
		PlaneVisual->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}

	SetActorRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));

	EnviromentMirrorer = CreateDefaultSubobject<UEnviromentMirrorer>(TEXT("EnviromentMirrorer"));
	AddInstanceComponent(EnviromentMirrorer);
}

void AMirror::OnConstruction(const FTransform& Transform)
{
	if (GetPlanarReflectionActor() == nullptr && !HasAllFlags(RF_Transient))
	{
		TArray<AActor*> Attached;
		GetAttachedActors(Attached);

		APlanarReflection** SearchResult = nullptr;
		int32* SearchIndex = nullptr;
		bool Found = Attached.FindItemByClass(SearchResult, SearchIndex, 0);

		FVector Location(0.0f, 0.0f, 0.5f);
		FRotator Rotation(0.0f, 0.0f, 0.0f);
		FActorSpawnParameters SpawnInfo;
		APlanarReflection* PlanarReflection = GetWorld()->SpawnActor<APlanarReflection>(Location, Rotation, SpawnInfo);
		PlanarReflection->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		PlanarReflection->SetActorRelativeScale3D(FVector(0.025f, 0.025f, 1.0f));

		UPlanarReflectionComponent* ReflectionComponent = PlanarReflection->GetPlanarReflectionComponent();
		ReflectionComponent->ScreenPercentage = 100;
		ReflectionComponent->PrefilterRoughness = 0.0f;
		ReflectionComponent->DistanceFromPlaneFadeoutStart = 5.0f;
		ReflectionComponent->DistanceFromPlaneFadeoutEnd = 5.0f;
		ReflectionComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
	}

	if (GetSceneCapture2DActor() == nullptr && !HasAllFlags(RF_Transient))
	{
		TArray<AActor*> Attached;
		GetAttachedActors(Attached);

		ASceneCapture2D** SearchResult = nullptr;
		int32* SearchIndex = nullptr;
		bool Found = Attached.FindItemByClass(SearchResult, SearchIndex, 0);

		FVector Location(0.0f, 0.0f, 0.0f);
		FRotator Rotation(0.0f, 90.0f, 90.0f);
		FActorSpawnParameters SpawnInfo;
		ASceneCapture2D* SceneCapture = GetWorld()->SpawnActor<ASceneCapture2D>(Location, Rotation, SpawnInfo);
		SceneCapture->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
		SceneCapture->SetActorRelativeScale3D(FVector(0.2f, 0.2f, 1.0f));
	}
}

// Called when the game starts or when spawned
void AMirror::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMirror::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMirror::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Collision Occured"));
}