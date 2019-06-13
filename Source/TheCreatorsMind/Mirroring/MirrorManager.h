// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Mirroring/Mirror.h"
#include "MirrorManager.generated.h"

#define MIRRORING_MAX_VALUE 4

UENUM()
enum MirroringType
{
	Horizontal = 1,
	Vertical = 2,
	Diagonal = 4
};

USTRUCT()
struct FMirrorInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	AMirror* Mirror;

	UPROPERTY(EditAnywhere)
	uint8 RoomIndex;
};

USTRUCT()
struct FRoomState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<AActor*> UniqueActors;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> ExcludedActors;
};

USTRUCT()
struct FStateSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FRoomState> StatesInOrder;

	UPROPERTY(EditAnywhere)
	TArray<FMirrorInfo> Mirrors;

	UPROPERTY(EditAnywhere)
	uint8 CurrentState = 0;
};

USTRUCT()
struct FRoom
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<MirroringType>, FStateSet> RoomStates;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<MirroringType> CurrentDirection;

	/*bool HasDefaultState()
	{
		return Mirrors.Num() == 0;
	}*/
};

USTRUCT()
struct FRoomSet
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FRoom> RoomsInOrder;

	UPROPERTY(EditAnywhere)
	TArray<FMirrorInfo> Mirrors;

	UPROPERTY(EditAnywhere)
	uint8 CurrentRoom = 0;
};

USTRUCT()
struct FSearchData
{
	GENERATED_USTRUCT_BODY()

	AMirror* Mirror;
	uint8 MirrorIndex;

	FRoomSet* RoomSet;
	TEnumAsByte<MirroringType> RoomSetDirection;

	FStateSet* StateSet;
	TEnumAsByte<MirroringType> StateSetDirection;

	bool ChangingRoom;
};

USTRUCT()
struct FMirrorProjectionInfo
{
	GENERATED_USTRUCT_BODY()

	AMirror* Mirror;
	FIntRect ViewScreen;
	FMatrix ViewProjectionMatrix;

	FMirrorProjectionInfo()
	{
		Mirror = nullptr;
	}

	FMirrorProjectionInfo(AMirror* mirror)
	{
		Mirror = mirror;
	}
};

USTRUCT()
struct FMirrorRenderStack
{
	GENERATED_USTRUCT_BODY()

	FMirrorProjectionInfo* CurrentMirror;
	FVector InVector;
	uint8 Level; //This will determine the screen capture render order (priority)

	TArray<FMirrorProjectionInfo*> ReflectedMirrors;
};

UCLASS()
class THECREATORSMIND_API AMirrorManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMirrorManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<MirroringType>, FRoomSet> RoomSets;

	UPROPERTY(EditAnywhere)
	ACharacter* Player;

	UPROPERTY(EditAnywhere)
	FName ManagerTagName;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<MirroringType> InitalDirection;

	//bool Flipped = false;

	TArray<AActor*> BeingFlipped;
	TArray<AActor*> UniqueActors;
	TArray<AActor*> CurrentlyOwned;

	TArray<FMirrorProjectionInfo*> MirrorRenderingInfo;

	void InitializeMirroring();
	void InitializeCollision();
	void InitializeUtilityValues();
	void InitializeVisibility();

	void HandleCollisionWith(AMirror* mirror);
	void UpdateMirrorsIn(FSearchData& search);
	void PerformDiagonalSwap(FSearchData& horizontalData, FSearchData& verticalData);
	void FindMirrorContainer(FSearchData& search);
	void SetHiddenInReflection(FMirrorInfo& mirror, TArray<AActor*> hidden);
	void SetHiddenToPlayer(TArray<AActor*> hidden);
	bool AssertValid(const FSearchData& search);
	bool AssertSwappable(const FSearchData& search1, const FSearchData& search2);

	FVector GetReflectedActorLocation(const FVector ReflectionNormal, const FVector MirrorLocation, const FVector ActorLocation) const;
	FRotator GetReflectedActorRotation(const FVector ReflectionNormal, const FVector ActorNormal, FRotator ActorRotation) const;
	void ReflectActor(AActor* Actor);

	void ReadyRendering(const float DeltaTime);
	//TArray<FMirrorRenderStack> GetRendering(const float DeltaTime); //Will only allow mirrors to be rendered by a single other mirror most likely at first by keeping a list of mirrors that have not been assigned yet
	void GetRawRenderingLists(const float DeltaTime, TArray<FMirrorProjectionInfo*>& BaseMirrors, TArray<FMirrorProjectionInfo*>& OtherMirrors);
	bool IsProjectedFrom(const FMirrorProjectionInfo* Base, const FMirrorProjectionInfo* Other);
	bool IsProjectedFromPlayer(const APlayerController* Controller, const AMirror* Mirror);
	//void ProjectOnto(const FMirrorRenderStack& Base, FMirrorRenderStack& Other); //This adjusts the rotation of the SceneCapture2D on 'Other' and updates the InVector of 'Other'
	void ProjectOnto(const FMirrorProjectionInfo* Base, FMirrorProjectionInfo* Other); //This adjusts the rotation of the SceneCapture2D on 'Other' and updates the InVector of 'Other'
	//void ProjectOnto(const AActor* Base, const AActor* LookReflector, AActor* Other);
	void ProjectOnto(const FVector& In, const FVector& LookReflector, AActor* Other);
	void BuildProjectionMatrix(const ASceneCapture2D* SceneCapture, FMatrix& ViewProjectionMatrix);
	void BuildProjectionMatrix(const FIntPoint RenderTargetSize, const ECameraProjectionMode::Type ProjectionType, const float FOV, const float InOrthoWidth, FMatrix& ProjectionMatrix);
	bool ProjectWorldToScreen(const FMatrix& ViewProjectionMatrix, const FIntRect& ViewRect, const FVector& WorldPosition, FVector2D& ScreenPosition, const bool bPlayerViewportRelative);
	void UpdateMirrorProjection();

	//void BuildReflectionMatrix(const FVector4 Plane);
	//FVector4 BuildSpacePlane(const FMatrix& WorldToActorMatrix, const FVector& Position, const FVector& Normal, const float SideSign);
	//void BuildObliqueMatrix(FMatrix& Projection, const FVector4& ClipPlane);

	//FPlane& GetMirrorReflectionPlane(const AMirror* Mirror);
	//FMatrix& BuildReflectionMatrix(const FMatrix& ReflectionPlane);

	//TEnumAsByte<MirroringType> GetActualValue(TEnumAsByte<MirroringType> mirroringType);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);
};
