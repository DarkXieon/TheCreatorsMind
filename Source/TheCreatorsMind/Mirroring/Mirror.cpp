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
			//PlaneVisual->SetMaterial(0, MirrorMaterial.Object);
			MirrorMaterial = MirrorMaterialInterface.Object;
		}

		const FString materialName = FString::Printf(TEXT("MaterialInstanceConstant'/Game/Materials/MirrorMaterials/InstancedTextureMaterials/MirrorTextureMaterial_%d'"), TextureId);
		static ConstructorHelpers::FObjectFinder<UMaterialInstance> TextureMaterialInterface(*materialName);
		if (TextureMaterialInterface.Succeeded())
		{
			//PlaneVisual->SetMaterial(0, MirrorMaterial.Object);
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

	//ChildReflection = CreateDefaultSubobject<UChildReflection>(TEXT("Planar Reflection"));
	//AddInstanceComponent(ChildReflection);
	//ChildReflection->RegisterComponent();

	//ChildCapture = CreateDefaultSubobject<UChildCapture>(TEXT("Scene Capture"));
	//AddInstanceComponent(ChildCapture);
	//ChildCapture->RegisterComponent();
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
		PlanarReflection->SetActorRelativeScale3D(FVector(0.025f, 0.025f, 1.0f)); //FVector(1.0f, 1.0f, 1.0f));

		UPlanarReflectionComponent* ReflectionComponent = PlanarReflection->GetPlanarReflectionComponent();
		ReflectionComponent->ScreenPercentage = 100;
		ReflectionComponent->PrefilterRoughness = 0.0f;
		ReflectionComponent->DistanceFromPlaneFadeoutStart = 5.0f;
		ReflectionComponent->DistanceFromPlaneFadeoutEnd = 5.0f;
		ReflectionComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
		//ReflectionComponent->UpdatePreviewShape();
		/*ReflectionComponent->DistanceFromPlaneFadeoutStart = 0.0f;
		ReflectionComponent->DistanceFromPlaneFadeoutEnd = 1.0f;*/
		//ReflectionComponent->AngleFromPlaneFadeStart = 90;
		//ReflectionComponent->AngleFromPlaneFadeEnd = 90;
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
//
//	//TArray<AActor*> tempChildActors;
//	//GetAttachedActors(tempChildActors);
//	//if (!tempChildActors.ContainsByPredicate([](AActor* actor) { return actor->IsA(APlanarReflection::StaticClass()); }) && !HasAllFlags(RF_Transient))
//	//{
//	//	FVector Location(0.0f, 0.0f, 0.5f);
//	//	FRotator Rotation(0.0f, 0.0f, 0.0f);
//	//	FActorSpawnParameters SpawnInfo;
//	//	APlanarReflection* PlanarReflection = GetWorld()->SpawnActor<APlanarReflection>(Location, Rotation, SpawnInfo);
//	//	PlanarReflection->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
//	//	PlanarReflection->SetActorRelativeScale3D(FVector(0.025f, 0.025f, 1.0f));//FVector(1.0f, 1.0f, 1.0f));
//	//	
//	//	UPlanarReflectionComponent* ReflectionComponent = PlanarReflection->GetPlanarReflectionComponent();
//	//	ReflectionComponent->ScreenPercentage = 100;
//	//	ReflectionComponent->PrefilterRoughness = 0.0f;
//	//	ReflectionComponent->DistanceFromPlaneFadeoutStart = 5.0f;
//	//	ReflectionComponent->DistanceFromPlaneFadeoutEnd = 5.0f;
//	//	ReflectionComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
//	//	//ReflectionComponent->UpdatePreviewShape();
//	//	/*ReflectionComponent->DistanceFromPlaneFadeoutStart = 0.0f;
//	//	ReflectionComponent->DistanceFromPlaneFadeoutEnd = 1.0f;*/
//	//	//ReflectionComponent->AngleFromPlaneFadeStart = 90;
//	//	//ReflectionComponent->AngleFromPlaneFadeEnd = 90;
//	//}
}

// Called when the game starts or when spawned
void AMirror::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Began play"));
}

bool IsRendering(AMirror* other, USceneCaptureComponent2D* CaptureComponent, const FIntRect& screenBounds);
bool IsFrontFacing(AMirror* other, USceneCaptureComponent2D* CaptureComponent);
bool ProjectWorldToScreen(const FMatrix& ViewProjectionMatrix, const FIntRect& ViewRect, const FVector& WorldPosition, FVector2D& ScreenPosition, bool bPlayerViewportRelative);
void BuildProjectionMatrix(FIntPoint RenderTargetSize, ECameraProjectionMode::Type ProjectionType, float FOV, float InOrthoWidth, FMatrix& ProjectionMatrix);
// Called every frame
void AMirror::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//APlayerController* playerController = nullptr;
	//USceneCaptureComponent2D* capture = nullptr;

	//for (TActorIterator<AActor> CurrentActor(GetWorld()); CurrentActor; ++CurrentActor)
	//{
	//	if (CurrentActor->IsA<APlayerController>())
	//	{
	//		playerController = (APlayerController*)*CurrentActor;
	//	}
	//	else if (CurrentActor->GetComponentByClass(USceneCaptureComponent2D::StaticClass()) && CurrentActor->GetOwner() == nullptr)
	//	{
	//		capture = (USceneCaptureComponent2D*)CurrentActor->GetComponentByClass(USceneCaptureComponent2D::StaticClass());
	//	}
	//}

	//if (DisplayTest && playerController /*&& TestMirror && TestCapture*/ && capture)
	//{
	//	int32 screenWidth;
	//	int32 screenHeight;
	//	playerController->GetViewportSize(screenWidth, screenHeight);
	//	FIntRect Rect = FIntRect(0, 0, screenWidth, screenHeight);
	//	
	//	bool isOnScreen = IsRendering(this, capture, Rect);

	//	FString onscreen = isOnScreen ? TEXT("true") : TEXT("false");
	//	UE_LOG(LogTemp, Warning, TEXT("Is on screen: %s"), *onscreen);

	//	FString wasrendered = WasRecentlyRendered(DeltaTime) ? TEXT("true") : TEXT("false");
	//	UE_LOG(LogTemp, Warning, TEXT("Was recently rendered: %s"), *wasrendered);

	//	//int32 screenWidth;
	//	//int32 screenHeight;
	//	//FVector2D screenLocation;
	//	//FVector location = GetActorLocation();
	//	//
	//	////APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	//	////APlayerController* playerController = (APlayerController*)Player->Controller;

	//	//int32 screenX = (int32)screenLocation.X;
	//	//int32 screenY = (int32)screenLocation.Y;

	//	////playerController->proje
	//	//bool projected = playerController->ProjectWorldLocationToScreen(location, screenLocation);
	//	//playerController->GetViewportSize(screenWidth, screenHeight);

	//	//bool isOnScreen = projected && screenX >= 0 && screenY >= 0 && screenX < screenWidth && screenY < screenHeight;

	//	//FString onscreen = isOnScreen ? TEXT("true") : TEXT("false");
	//	//UE_LOG(LogTemp, Warning, TEXT("Is on screen: %s"), *onscreen);
	//}


	//USceneCaptureComponent2D* test;
	//FScene test2;
	//test->
	//test->CustomProjectionMatrix
}

bool IsRendering(AMirror* other, USceneCaptureComponent2D* CaptureComponent, const FIntRect& screenBounds)
{
	UE_LOG(LogTemp, Warning, TEXT("Test"));

	if (CaptureComponent->TextureTarget != nullptr)
	{
		//// Only ensure motion blur cache is up to date when doing USceneCaptureComponent2D::CaptureScene(),
		//// but only when bAlwaysPersistRenderingState == true for backward compatibility.
		//if (!CaptureComponent->bCaptureEveryFrame && CaptureComponent->bAlwaysPersistRenderingState)
		//{
		//	// We assume the world is not paused since the CaptureScene() has manually been called.
		//	EnsureMotionBlurCacheIsUpToDate(false);
		//}

		FTransform Transform = CaptureComponent->GetComponentTransform(); //GetComponentToWorld();
		FVector ViewLocation = Transform.GetTranslation();

		// Remove the translation from Transform because we only need rotation.
		Transform.SetTranslation(FVector::ZeroVector);
		Transform.SetScale3D(FVector::OneVector);
		FMatrix ViewRotationMatrix = Transform.ToInverseMatrixWithScale();

		// swap axis st. x=z,y=x,z=y (unreal coord space) so that z is up
		ViewRotationMatrix = ViewRotationMatrix * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1));
		const float FOV = CaptureComponent->FOVAngle * (float)PI / 360.0f;
		FIntPoint CaptureSize(CaptureComponent->TextureTarget->GetSurfaceWidth(), CaptureComponent->TextureTarget->GetSurfaceHeight());

		FMatrix ProjectionMatrix;
		if (CaptureComponent->bUseCustomProjectionMatrix)
		{
			ProjectionMatrix = CaptureComponent->CustomProjectionMatrix;
		}
		else
		{
			BuildProjectionMatrix(CaptureSize, CaptureComponent->ProjectionType, FOV, CaptureComponent->OrthoWidth, ProjectionMatrix);
		}

		//added
		int32 screenWidth = CaptureComponent->TextureTarget->GetSurfaceWidth();
		int32 screenHeight = CaptureComponent->TextureTarget->GetSurfaceHeight(); 
		FVector origin, extents;
		other->GetActorBounds(false, origin, extents);
		FIntRect screenBounds2 = FIntRect(0, 0, screenWidth, screenHeight);
		//FIntRect screenBounds2 = FIntRect(ViewLocation.X - screenWidth / 2, ViewLocation.Y - screenHeight / 2, ViewLocation.X + screenWidth / 2, ViewLocation.Y + screenHeight / 2);
		FVector2D screenLocation;
		FVector location = other->GetActorLocation(); 
		//FVector Origin = CaptureComponent->GetComponentLocation();

		//UE_LOG(LogTemp, Warning, TEXT("screen width: %d screen height: %d"), screenWidth, screenHeight);

		FMatrix viewProjectionMatrix = FTranslationMatrix(-ViewLocation) * ViewRotationMatrix * ProjectionMatrix;
		
		bool projected = ProjectWorldToScreen(viewProjectionMatrix/*ProjectionMatrix*/, screenBounds2, location, screenLocation, true);

		int32 screenX = (int32)screenLocation.X;
		int32 screenY = (int32)screenLocation.Y;

		//UE_LOG(LogTemp, Warning, TEXT("screen x: %d screen y: %d"), screenX, screenY);

		//bool isOnScreen = projected && screenX >= 0 && screenY >= 0 && screenX < screenWidth && screenY < screenHeight;
		bool isOnScreen = projected && screenX + extents.X >= 0 && screenY + extents.Y >= 0 && screenX - extents.X < screenWidth && screenY - extents.Y < screenHeight && IsFrontFacing(other, CaptureComponent);

		FString onscreen = isOnScreen ? TEXT("true") : TEXT("false");
		UE_LOG(LogTemp, Warning, TEXT("Is on screen: %s"), *onscreen);

		return isOnScreen;
	}

	return false;
}

bool IsFrontFacing(AMirror* other, USceneCaptureComponent2D* CaptureComponent)
{
	FVector from = CaptureComponent->GetOwner()->GetActorRightVector();
	FVector to = other->GetActorRightVector();

	float angle = FVector::DotProduct(from, to);
	//float result = FMath::Cos(angle);

	//UE_LOG(LogTemp, Warning, TEXT("Angle: %f Cosine: %f"), angle, result);

	return angle >= 0.0f;
}

bool ProjectWorldToScreen(const FMatrix& ViewProjectionMatrix, const FIntRect& ViewRect, const FVector& WorldPosition, FVector2D& ScreenPosition, bool bPlayerViewportRelative)
{
	
	bool bResult = FSceneView::ProjectWorldToScreen(WorldPosition, ViewRect, ViewProjectionMatrix, ScreenPosition);

	/*if (bPlayerViewportRelative)
	{
		ScreenPosition -= FVector2D(ProjectionData.GetConstrainedViewRect().Min);
	}*/

	//bResult = bResult && Player->PostProcessWorldToScreen(WorldPosition, ScreenPosition, bPlayerViewportRelative);
	return bResult;
}

void BuildProjectionMatrix(FIntPoint RenderTargetSize, ECameraProjectionMode::Type ProjectionType, float FOV, float InOrthoWidth, FMatrix& ProjectionMatrix)
{
	float const XAxisMultiplier = 1.0f;
	float const YAxisMultiplier = RenderTargetSize.X / (float)RenderTargetSize.Y;

	if (ProjectionType == ECameraProjectionMode::Orthographic)
	{
		check((int32)ERHIZBuffer::IsInverted);
		const float OrthoWidth = InOrthoWidth / 2.0f;
		const float OrthoHeight = InOrthoWidth / 2.0f * XAxisMultiplier / YAxisMultiplier;

		const float NearPlane = 0;
		const float FarPlane = WORLD_MAX / 8.0f;

		const float ZScale = 1.0f / (FarPlane - NearPlane);
		const float ZOffset = -NearPlane;

		ProjectionMatrix = FReversedZOrthoMatrix(
			OrthoWidth,
			OrthoHeight,
			ZScale,
			ZOffset
		);
	}
	else
	{
		if ((int32)ERHIZBuffer::IsInverted)
		{
			ProjectionMatrix = FReversedZPerspectiveMatrix(
				FOV,
				FOV,
				XAxisMultiplier,
				YAxisMultiplier,
				GNearClippingPlane,
				GNearClippingPlane
			);
		}
		else
		{
			ProjectionMatrix = FPerspectiveMatrix(
				FOV,
				FOV,
				XAxisMultiplier,
				YAxisMultiplier,
				GNearClippingPlane,
				GNearClippingPlane
			);
		}
	}
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