// Fill out your copyright notice in the Description page of Project Settings.

#include "MirrorManager.h"
#include "MirroringBehaviour.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PlanarReflectionComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "EngineUtils.h"

//TODO: Make sure all components transforms are set to moveable
//      Make sure all actors have a component that handels events on being shown--if not then add the component with default values
//      Make sure all actors generate overlap events
//		Make sure all actors have "owner no see" set to true

// Sets default values
AMirrorManager::AMirrorManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMirrorManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeMirroring();
}

void AMirrorManager::InitializeMirroring()
{
	//UE_LOG(LogTemp, Warning, TEXT("InitializeUtilityValues beginning. . ."))
	InitializeUtilityValues();
	//UE_LOG(LogTemp, Warning, TEXT("InitializeUtilityValues finished. . ."))

	//UE_LOG(LogTemp, Warning, TEXT("InitializeCollision beginning. . ."))
	InitializeCollision();
	//UE_LOG(LogTemp, Warning, TEXT("InitializeCollision finished. . ."))

	//UE_LOG(LogTemp, Warning, TEXT("InitializeVisibility beginning. . ."))
	InitializeVisibility();
	//UE_LOG(LogTemp, Warning, TEXT("InitializeVisibility finished. . ."))
}

void AMirrorManager::InitializeVisibility()
{
	for (int i = 0; i < BeingFlipped.Num(); i++)
	{
		UMirroringBehaviour* behaviourComponent = (UMirroringBehaviour*)BeingFlipped[i]->GetComponentByClass(UMirroringBehaviour::StaticClass());

		if (behaviourComponent == nullptr)
		{
			UMirroringBehaviour* mirroringBehaviour = NewObject<UMirroringBehaviour>(BeingFlipped[i]);

			mirroringBehaviour->RegisterComponent();

			mirroringBehaviour->Initialize();
		}
	}

	TArray<AActor*> Mirrors = BeingFlipped.FilterByPredicate([](AActor* actor) { return actor->IsA<AMirror>(); });

	for (int i = 0; i < Mirrors.Num(); i++)
	{
		AMirror* currentMirror = (AMirror*)Mirrors[i];

		FSearchData updating;
		updating.Mirror = currentMirror;
		FindMirrorContainer(updating);

		if (!updating.ChangingRoom)
		{
			FMirrorInfo& foundMirror = updating.StateSet->Mirrors[updating.MirrorIndex];

			SetHiddenInReflection(foundMirror, updating.StateSet->StatesInOrder[foundMirror.RoomIndex].ExcludedActors);
		}
		else
		{
			FMirrorInfo& foundMirror = updating.RoomSet->Mirrors[updating.MirrorIndex];

			FRoom& mirrorRoom = updating.RoomSet->RoomsInOrder[foundMirror.RoomIndex];
			FStateSet& mirrorStateSet = mirrorRoom.RoomStates[mirrorRoom.CurrentDirection];

			SetHiddenInReflection(foundMirror, mirrorStateSet.StatesInOrder[mirrorStateSet.CurrentState].ExcludedActors);
		}
	}

	FRoom& initialRoom = RoomSets[InitalDirection].RoomsInOrder[RoomSets[InitalDirection].CurrentRoom];
	FStateSet& initialStateSet = initialRoom.RoomStates[initialRoom.CurrentDirection];

	SetHiddenToPlayer(initialStateSet.StatesInOrder[initialStateSet.CurrentState].ExcludedActors);
}

void AMirrorManager::InitializeUtilityValues()
{
	for (TActorIterator<AActor> CurrentActor(GetWorld()); CurrentActor; ++CurrentActor)
	{
		if (CurrentActor->ActorHasTag(ManagerTagName))
		{
			BeingFlipped.Add(*CurrentActor);
		}
	}

	for (int i = 1; i < MIRRORING_MAX_VALUE + 1; i *= 2)
	{
		TEnumAsByte<MirroringType> current = (MirroringType)i;

		if (RoomSets.Contains(current))
		{
			FRoomSet& currentRoomSet = RoomSets[current];

			for (int j = 0; j < currentRoomSet.RoomsInOrder.Num(); j++)
			{
				FRoom& currentRoom = currentRoomSet.RoomsInOrder[j];

				for (int f = 1; f < MIRRORING_MAX_VALUE + 1; f *= 2)
				{
					TEnumAsByte<MirroringType> nestedCurrent = (MirroringType)f;

					if (currentRoom.RoomStates.Contains(nestedCurrent))
					{
						FStateSet& stateSet = currentRoom.RoomStates[nestedCurrent];

						for (int h = 0; h < stateSet.StatesInOrder.Num(); h++)
						{
							FRoomState& roomState = stateSet.StatesInOrder[h];

							UE_LOG(LogTemp, Warning, TEXT("Unique actors for %s room state %d is %d elements long. . ."), *GetName(), h, roomState.UniqueActors.Num())

							for (int k = 0; k < roomState.UniqueActors.Num(); k++)
							{
								UniqueActors.AddUnique(roomState.UniqueActors[k]);
							}
						}
					}
				}

				/*for (int f = 1; f < MIRRORING_MAX_VALUE + 1; f *= 2)
				{
					TEnumAsByte<MirroringType> nestedCurrent = (MirroringType)f;

					if (currentRoom.RoomStates.Contains(nestedCurrent))
					{
						FStateSet& stateSet = currentRoom.RoomStates[nestedCurrent];

						for (int h = 0; h < stateSet.StatesInOrder.Num(); h++)
						{
							FRoomState& roomState = stateSet.StatesInOrder[h];

							roomState.ExcludedActors = BeingFlipped.FilterByPredicate([this, roomState](AActor* actor) { return UniqueActors.Contains(actor) && !roomState.UniqueActors.Contains(actor); });

							UE_LOG(LogTemp, Warning, TEXT("Excluded actors for %s room state %d is %d elements long. . ."), *GetName(), h, roomState.ExcludedActors.Num())
						}
					}
				}*/
			}
		}
	}

	for (int i = 1; i < MIRRORING_MAX_VALUE + 1; i *= 2)
	{
		TEnumAsByte<MirroringType> current = (MirroringType)i;

		if (RoomSets.Contains(current))
		{
			FRoomSet& currentRoomSet = RoomSets[current];

			for (int j = 0; j < currentRoomSet.RoomsInOrder.Num(); j++)
			{
				FRoom& currentRoom = currentRoomSet.RoomsInOrder[j];

				for (int f = 1; f < MIRRORING_MAX_VALUE + 1; f *= 2)
				{
					TEnumAsByte<MirroringType> nestedCurrent = (MirroringType)f;

					if (currentRoom.RoomStates.Contains(nestedCurrent))
					{
						FStateSet& stateSet = currentRoom.RoomStates[nestedCurrent];

						for (int h = 0; h < stateSet.StatesInOrder.Num(); h++)
						{
							FRoomState& roomState = stateSet.StatesInOrder[h];

							roomState.ExcludedActors = BeingFlipped.FilterByPredicate([this, roomState](AActor* actor) { return UniqueActors.Contains(actor) && !roomState.UniqueActors.Contains(actor); });

							UE_LOG(LogTemp, Warning, TEXT("Excluded actors for %s room state %d is %d elements long. . ."), *GetName(), h, roomState.ExcludedActors.Num())
						}
					}
				}
			}
		}
	}
}

void AMirrorManager::InitializeCollision()
{
	for (int i = 1; i < MIRRORING_MAX_VALUE + 1; i *= 2)
	{
		TEnumAsByte<MirroringType> current = (MirroringType)i;

		if (RoomSets.Contains(current))
		{
			FRoomSet& currentRoomSet = RoomSets[current];

			for (int j = 0; j < currentRoomSet.Mirrors.Num(); j++)
			{

				if (currentRoomSet.Mirrors[j].Mirror != nullptr)
				{
					currentRoomSet.Mirrors[j].Mirror->OnActorBeginOverlap.AddUniqueDynamic(this, &AMirrorManager::OnOverlapBegin);
				}
			}

			for (int j = 0; j < currentRoomSet.RoomsInOrder.Num(); j++)
			{
				FRoom& currentRoom = currentRoomSet.RoomsInOrder[j];

				for (int f = 1; f < MIRRORING_MAX_VALUE + 1; f *= 2)
				{
					TEnumAsByte<MirroringType> nestedCurrent = (MirroringType)f;

					if (currentRoom.RoomStates.Contains(nestedCurrent))
					{
						FStateSet& stateSet = currentRoom.RoomStates[nestedCurrent];

						for (int h = 0; h < stateSet.Mirrors.Num(); h++)
						{
							if (stateSet.Mirrors[h].Mirror != nullptr)
							{
								stateSet.Mirrors[h].Mirror->OnActorBeginOverlap.AddUniqueDynamic(this, &AMirrorManager::OnOverlapBegin);
							}
						}
					}
				}
			}
		}
	}
}

// Called every frame
void AMirrorManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ReadyRendering(DeltaTime);
}

void AMirrorManager::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OverlappedActor->IsA<AMirror>() || !OtherActor->IsA<ACharacter>())
	{
		UE_LOG(LogTemp, Fatal, TEXT("On Overlap being called without proper actors (a mirror and a pawn)"));
	}
	else
	{
		HandleCollisionWith((AMirror*)OverlappedActor);
	}
}

void AMirrorManager::HandleCollisionWith(AMirror* mirror)
{
	FVector reflectionNormal = mirror->GetActorRightVector();

	FVector reflectedLocation = GetReflectedActorLocation(reflectionNormal, mirror->GetActorLocation(), Player->GetActorLocation());
	Player->SetActorLocation(reflectedLocation);

	for (AActor* toReflect : BeingFlipped)
	{
		FVector reflectedLocation = GetReflectedActorLocation(reflectionNormal, mirror->GetActorLocation(), toReflect->GetActorLocation());
		FRotator reflectedRotation = GetReflectedActorRotation(reflectionNormal, toReflect->GetActorRightVector(), toReflect->GetActorRotation());
		
		/*if (toReflect == mirror)
		{
			UE_LOG(LogTemp, Warning, TEXT("The original location is (%f, %f, %f)"), toReflect->GetActorLocation().X, toReflect->GetActorLocation().Y, toReflect->GetActorLocation().Z);
			UE_LOG(LogTemp, Warning, TEXT("The reflected location is (%f, %f, %f)"), reflectedLocation.X, reflectedLocation.Y, reflectedLocation.Z);
			UE_LOG(LogTemp, Warning, TEXT("The original rotation is (%f, %f, %f)"), toReflect->GetActorRotation().Roll, toReflect->GetActorRotation().Pitch, toReflect->GetActorRotation().Yaw);
			UE_LOG(LogTemp, Warning, TEXT("The reflected rotation is (%f, %f, %f)"), reflectedRotation.Roll, reflectedRotation.Pitch, reflectedRotation.Yaw);
		}*/

		/*FTransform toReflectTransform = toReflect->GetActorTransform();
		toReflectTransform.SetLocation(reflectedLocation);
		toReflectTransform.Mirror(EAxis::None, EAxis::X);
		toReflect->SetActorTransform(toReflectTransform);*/

		toReflect->SetActorLocation(reflectedLocation);
		toReflect->SetActorRotation(reflectedRotation);
		ReflectActor(toReflect);
	}

	FSearchData updating;
	updating.Mirror = mirror;
	FindMirrorContainer(updating);
	UpdateMirrorsIn(updating);
}

void AMirrorManager::UpdateMirrorsIn(FSearchData& search)
{
	if (!search.ChangingRoom)
	{
		FMirrorInfo& collidedWithMirrorInfo = search.StateSet->Mirrors[search.MirrorIndex];

		if (search.StateSet->Mirrors.Num() > 1)
		{
			uint8 secondMirrorIndex = search.MirrorIndex == 0 ? 1 : 0;
			
			uint8 secondMirrorStateIndex = search.StateSet->CurrentState > collidedWithMirrorInfo.RoomIndex
				? (search.StateSet->CurrentState + 1 - 3 + search.StateSet->StatesInOrder.Num()) % search.StateSet->StatesInOrder.Num()
				: (search.StateSet->CurrentState + 1 + 3 + search.StateSet->StatesInOrder.Num()) % search.StateSet->StatesInOrder.Num();

			search.StateSet->Mirrors[secondMirrorIndex].RoomIndex = secondMirrorStateIndex;

			SetHiddenInReflection(search.StateSet->Mirrors[secondMirrorIndex], search.StateSet->StatesInOrder[secondMirrorStateIndex].ExcludedActors);
		}

		uint8 temp = collidedWithMirrorInfo.RoomIndex;
		collidedWithMirrorInfo.RoomIndex = search.StateSet->CurrentState;
		search.StateSet->CurrentState = temp;

		search.RoomSet->RoomsInOrder[search.RoomSet->CurrentRoom].CurrentDirection = search.StateSetDirection;

		SetHiddenInReflection(collidedWithMirrorInfo, search.StateSet->StatesInOrder[collidedWithMirrorInfo.RoomIndex].ExcludedActors);
		SetHiddenToPlayer(search.StateSet->StatesInOrder[search.StateSet->CurrentState].ExcludedActors);
	}
	else
	{
		FMirrorInfo& collidedWithMirrorInfo = search.RoomSet->Mirrors[search.MirrorIndex];

		if (search.RoomSet->Mirrors.Num() > 1)
		{
			uint8 secondMirrorIndex = search.MirrorIndex == 0 ? 1 : 0;

			uint8 secondMirrorRoomIndex = search.RoomSet->CurrentRoom > collidedWithMirrorInfo.RoomIndex
				? (search.RoomSet->CurrentRoom + 1 - 3 + search.RoomSet->RoomsInOrder.Num()) % search.RoomSet->RoomsInOrder.Num()
				: (search.RoomSet->CurrentRoom + 1 + 3 + search.RoomSet->RoomsInOrder.Num()) % search.RoomSet->RoomsInOrder.Num();

			search.RoomSet->Mirrors[secondMirrorIndex].RoomIndex = secondMirrorRoomIndex;

			FRoom& secondMirrorRoom = search.RoomSet->RoomsInOrder[secondMirrorRoomIndex];
			FStateSet& secondMirrorStateSet = secondMirrorRoom.RoomStates[secondMirrorRoom.CurrentDirection];

			SetHiddenInReflection(search.RoomSet->Mirrors[secondMirrorIndex], secondMirrorStateSet.StatesInOrder[secondMirrorStateSet.CurrentState].ExcludedActors);
		}

		uint8 temp = collidedWithMirrorInfo.RoomIndex;
		collidedWithMirrorInfo.RoomIndex = search.RoomSet->CurrentRoom;
		search.RoomSet->CurrentRoom = temp;

		FRoom& mirrorRoom = search.RoomSet->RoomsInOrder[collidedWithMirrorInfo.RoomIndex];
		FStateSet& mirrorStateSet = mirrorRoom.RoomStates[mirrorRoom.CurrentDirection];

		FRoom& currentRoom = search.RoomSet->RoomsInOrder[search.RoomSet->CurrentRoom];
		FStateSet& currentStateSet = currentRoom.RoomStates[currentRoom.CurrentDirection];

		SetHiddenInReflection(collidedWithMirrorInfo, mirrorStateSet.StatesInOrder[mirrorStateSet.CurrentState].ExcludedActors);
		SetHiddenToPlayer(currentStateSet.StatesInOrder[currentStateSet.CurrentState].ExcludedActors);
	}
}

void AMirrorManager::PerformDiagonalSwap(FSearchData& horizontalData, FSearchData& verticalData)
{

}

void AMirrorManager::FindMirrorContainer(FSearchData& search)
{
	for (auto currentRoomSet = RoomSets.CreateIterator(); currentRoomSet; ++currentRoomSet)
	{
		if (currentRoomSet.Value().Mirrors.ContainsByPredicate([search](FMirrorInfo mirrorInfo) { return mirrorInfo.Mirror == search.Mirror; }))
		{
			search.MirrorIndex = currentRoomSet.Value().Mirrors.IndexOfByPredicate([search](FMirrorInfo mirrorInfo) { return mirrorInfo.Mirror == search.Mirror; });

			search.RoomSetDirection = currentRoomSet.Key();
			search.RoomSet = &currentRoomSet.Value();

			FRoom& currentRoom = currentRoomSet.Value().RoomsInOrder[currentRoomSet.Value().CurrentRoom];

			search.StateSet = &currentRoom.RoomStates[currentRoom.CurrentDirection];
			search.ChangingRoom = true;

			return;
		}
		else
		{
			for (int i = 0; i < currentRoomSet.Value().RoomsInOrder.Num(); i++)
			{
				FRoom& currentRoom = currentRoomSet.Value().RoomsInOrder[i];

				for (auto currentStateSet = currentRoom.RoomStates.CreateIterator(); currentStateSet; ++currentStateSet)
				{
					if (currentStateSet.Value().Mirrors.ContainsByPredicate([search](FMirrorInfo mirrorInfo) { return mirrorInfo.Mirror == search.Mirror; }))
					{
						search.MirrorIndex = currentStateSet.Value().Mirrors.IndexOfByPredicate([search](FMirrorInfo mirrorInfo) { return mirrorInfo.Mirror == search.Mirror; });

						search.RoomSetDirection = currentRoomSet.Key();
						search.RoomSet = &currentRoomSet.Value();

						search.StateSetDirection = currentStateSet.Key();
						search.StateSet = &currentStateSet.Value();
						search.ChangingRoom = false;

						return;
					}
				}
			}
		}
	}
}

FVector AMirrorManager::GetReflectedActorLocation(const FVector ReflectionNormal, const FVector MirrorLocation, const FVector ActorLocation) const
{
	FVector2D Q = FVector2D(MirrorLocation.X, MirrorLocation.Y);
	FVector2D P = FVector2D(ActorLocation.X, ActorLocation.Y);
	
	float x = P.Y * ReflectionNormal.X * ReflectionNormal.Y + 
			  P.X * ReflectionNormal.X * ReflectionNormal.X + 
			  Q.X * ReflectionNormal.Y * ReflectionNormal.Y - 
			  Q.Y * ReflectionNormal.X * ReflectionNormal.Y;

	float y = P.X * ReflectionNormal.X * ReflectionNormal.Y +
			  Q.Y * ReflectionNormal.X * ReflectionNormal.X +
			  P.Y * ReflectionNormal.Y * ReflectionNormal.Y -
			  Q.X * ReflectionNormal.X * ReflectionNormal.Y;

	FVector2D X = FVector2D(x, y);
	FVector2D Pprime = 2 * X - P;

	return FVector(Pprime.X, Pprime.Y, ActorLocation.Z);
}

FRotator AMirrorManager::GetReflectedActorRotation(const FVector ReflectionNormal, const FVector ActorNormal, FRotator ActorRotation) const//FQuat ActorRotation)
{
	/*ActorRotation = ActorRotation.Add(0.f, 180.f, 0.f);

	return ActorRotation;*/




	float x1 = FMath::RadiansToDegrees(FMath::Atan2(ReflectionNormal.Y, ReflectionNormal.X));
	float x2 = FMath::RadiansToDegrees(FMath::Atan2(ActorNormal.Y, ActorNormal.X));

	float rotation = FMath::RoundToFloat(2 * (x1 - x2));
	//rotation = (int)rotation % 360 == 0 ? 180.f : rotation; //if testing by uncommenting this--comment the '+ 180' below

	ActorRotation = ActorRotation.Add(0.f, rotation + 180, 0.f);

	return ActorRotation;





//	reflectedRotation = GetReflectedActorRotation(reflectionNormal, toReflect->GetActorRightVector(), toReflect->GetActorRotation());

//	int multipleTimes = (int)(toReflect->GetActorRotation().Pitch / 180.f);
//	float modAngle = FMath::Fmod(toReflect->GetActorRotation().Pitch, 180.f);
//	//float mult = FMath::IsNegativeFloat(modAngle) ? -1.f : 1.f;
//	float reflection = 180 * multipleTimes + (180.f - modAngle) - 90.f;
//	reflectedRotation = toReflect->GetActorRotation().Add(0.f, reflection, 0.f);

//  FVector reflectedScale = GetReflectedActorScale(toReflect->GetActorScale());

}

void AMirrorManager::SetHiddenToPlayer(TArray<AActor*> hidden)
{
	for (int i = 0; i < CurrentlyOwned.Num(); i++)
	{
		UMirroringBehaviour* mirroringBehaviour = (UMirroringBehaviour*)CurrentlyOwned[i]->GetComponentByClass(UMirroringBehaviour::StaticClass());

		CurrentlyOwned[i]->SetOwner(nullptr);
		mirroringBehaviour->WorldShow();
		UE_LOG(LogTemp, Warning, TEXT("%s is being shown to player"), *CurrentlyOwned[i]->GetName())
	}

	for (int i = 0; i < hidden.Num(); i++)
	{
		UMirroringBehaviour* mirroringBehaviour = (UMirroringBehaviour*)hidden[i]->GetComponentByClass(UMirroringBehaviour::StaticClass());

		hidden[i]->SetOwner(Player);
		mirroringBehaviour->WorldHide();
		UE_LOG(LogTemp, Warning, TEXT("%s is being hidden to player"), *hidden[i]->GetName())
	}

	CurrentlyOwned = hidden;
}

void AMirrorManager::SetHiddenInReflection(FMirrorInfo& mirror, TArray<AActor*> hidden)
{
	TArray<AActor*> attachedActors;
	mirror.Mirror->GetAttachedActors(attachedActors);

	APlanarReflection* planarReflection = (APlanarReflection*)*attachedActors.FindByPredicate([](AActor* actor) { return actor->IsA<APlanarReflection>(); });

	UPlanarReflectionComponent* reflectionComponent = planarReflection->GetPlanarReflectionComponent();
	reflectionComponent->HiddenActors = hidden;
}

void AMirrorManager::ReflectActor(AActor* actor)
{
	TArray<UActorComponent*> primitiveComponents = GetComponentsByClass(UStaticMeshComponent::StaticClass());

	for (UActorComponent* component : primitiveComponents)
	{
		FVector currentScale = GetActorScale();
		currentScale.X *= -1;
		SetActorScale3D(currentScale);

		bool culling = !((UStaticMeshComponent*)component)->bReverseCulling;
		((UStaticMeshComponent*)component)->SetReverseCulling(culling);
	}
}

bool AMirrorManager::AssertValid(const FSearchData& search)
{
	/*if (!search.HasRoomSet() || search.RoomSet->Mirrors.Num() <= 0 || search.RoomSet->Mirrors.Num() > 2)
	{
		UE_LOG(LogTemp, Fatal, TEXT("A mirror set has too many or too few elements. Please fix."));

		return false;
	}

	if (search.HasStateSet() && (search.StateSet->Mirrors.Num() <= 0 || search.StateSet->Mirrors.Num() > 2))
	{
		UE_LOG(LogTemp, Fatal, TEXT("A mirror set has too many or too few elements. Please fix."));

		return false;
	}*/

	return true;
}

bool AMirrorManager::AssertSwappable(const FSearchData& search1, const FSearchData& search2)
{
	/*if (!AssertValid(search1) || !AssertValid(search2))
	{
		return false;
	}

	if (search1.RoomSet->Mirrors.Num() != search2.RoomSet->Mirrors.Num())
	{
		UE_LOG(LogTemp, Fatal, TEXT("A pair of mirror room sets have uneven mirror amounts. Please fix."));

		return false;
	}

	if (search1.HasStateSet() == search2.HasStateSet() &&
		(search1.HasStateSet() && search1.StateSet->Mirrors.Num() != search2.StateSet->Mirrors.Num()))
	{
		UE_LOG(LogTemp, Fatal, TEXT("A pair of mirror room sets have uneven mirror amounts. Please fix."));

		return false;
	}
*/
	return true;
}

//TEnumAsByte<MirroringType> AMirrorManager::GetActualValue(TEnumAsByte<MirroringType> mirroringType)
//{
//	if ((mirroringType == MirroringType::Horizontal && !Flipped) || (mirroringType == MirroringType::Vertical && Flipped))
//	{
//		return MirroringType::Horizontal;
//	}
//	else if ((mirroringType == MirroringType::Vertical && !Flipped) || (mirroringType == MirroringType::Horizontal && Flipped))
//	{
//		return MirroringType::Vertical;
//	}
//	else
//	{
//		return MirroringType::Diagonal;
//	}
//}

//Projection

//TArray<FMirrorRenderStack> AMirrorManager::GetRendering(const float DeltaTime)
//{
//	TArray<FMirrorRenderStack> RenderStack;
//	TArray<FMirrorProjectionInfo*> BaseMirrors, OtherMirrors;
//	GetRawRenderingLists(DeltaTime, BaseMirrors, OtherMirrors);
//
//	for (auto CurrentProjectionInfo = BaseMirrors.CreateIterator(); CurrentProjectionInfo; ++CurrentProjectionInfo)
//	{
//		FMirrorProjectionInfo* CurrentInfo = *CurrentProjectionInfo;
//		AMirror* CurrentMirror = CurrentInfo->Mirror;
//		ASceneCapture2D* CurrentSceneCapture = CurrentMirror->GetSceneCapture2DActor();
//
//		ProjectOnto(Player, CurrentMirror, CurrentSceneCapture);
//
//		FMirrorRenderStack stackBase;
//		stackBase.CurrentMirror = CurrentInfo;
//		stackBase.Level = 0;
//		stackBase.
//	}
//
//	return TArray<FMirrorRenderStack>();
//}

void AMirrorManager::ReadyRendering(const float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("Readying rendering. . ."));

	UpdateMirrorProjection(); //Move this elsewhere, it's very costly here

	TArray<FMirrorProjectionInfo*> BaseMirrors, OtherMirrors, CurrentLevel, NextLevel;
	GetRawRenderingLists(DeltaTime, BaseMirrors, OtherMirrors);

	for (auto CurrentProjectionInfo = BaseMirrors.CreateIterator(); CurrentProjectionInfo; ++CurrentProjectionInfo)
	{
		FMirrorProjectionInfo* CurrentInfo = *CurrentProjectionInfo;
		AMirror* CurrentMirror = CurrentInfo->Mirror;
		ASceneCapture2D* CurrentSceneCapture = CurrentMirror->GetSceneCapture2DActor();
		APlanarReflection* CurrentPlanarReflection = CurrentMirror->GetPlanarReflectionActor();
		USceneCaptureComponent2D* CurrentCaptureComponent = CurrentSceneCapture->GetCaptureComponent2D();
		UPlanarReflectionComponent* CurrentReflectionComponent = CurrentPlanarReflection->GetPlanarReflectionComponent();
		
		//Original Code
		/**/
		CurrentCaptureComponent->bCaptureEveryFrame = false;
		CurrentCaptureComponent->bCaptureOnMovement = false;

		CurrentReflectionComponent->bCaptureEveryFrame = true;
		CurrentReflectionComponent->bCaptureOnMovement = true;
		CurrentReflectionComponent->CaptureSortPriority = 1;

		CurrentMirror->UseMirrorMaterial();
		/**/
		//End Original Code

		UE_LOG(LogTemp, Warning, TEXT("%s is a base mirror"), *CurrentMirror->GetName());

		//TODO: Make sure to change this project so it uses the player camera and not just the player

		//ProjectOnto(Player, CurrentMirror, CurrentSceneCapture);

		FVector relativePosition = CurrentMirror->GetActorLocation() - Player->GetActorLocation();
		//relativePosition.Y = 0.0f;
		relativePosition.Normalize();
		//UE_LOG(LogTemp, Warning, TEXT("Relative position: %f, %f, %f"), relativePosition.X, relativePosition.Y, relativePosition.Z);
		ProjectOnto(relativePosition, CurrentMirror->GetActorUpVector(), CurrentSceneCapture);

		NextLevel.Add(CurrentInfo);
	}

	int sortPriority = 2;

	while (NextLevel.Num() > 0 && OtherMirrors.Num() > 0)
	{
		CurrentLevel = TArray<FMirrorProjectionInfo*>(NextLevel);
		NextLevel.Empty();

		for (auto CurrentProjectionInfo = CurrentLevel.CreateIterator(); CurrentProjectionInfo; ++CurrentProjectionInfo)
		{
			FMirrorProjectionInfo* CurrentInfo = *CurrentProjectionInfo;
			AMirror* CurrentMirror = CurrentInfo->Mirror;
			ASceneCapture2D* CurrentSceneCapture = CurrentMirror->GetSceneCapture2DActor();
			APlanarReflection* CurrentPlanarReflection = CurrentMirror->GetPlanarReflectionActor();
			USceneCaptureComponent2D* CurrentCaptureComponent = CurrentSceneCapture->GetCaptureComponent2D();
			UPlanarReflectionComponent* CurrentReflectionComponent = CurrentPlanarReflection->GetPlanarReflectionComponent();

			//Original Code
			/**/if (!BaseMirrors.Contains(CurrentInfo))
			{
				CurrentCaptureComponent->bCaptureEveryFrame = true;
				CurrentCaptureComponent->bCaptureOnMovement = true;
				CurrentCaptureComponent->CaptureSortPriority = sortPriority;

				CurrentMirror->UseTextureMaterial();

				CurrentReflectionComponent->bCaptureEveryFrame = false;
				CurrentReflectionComponent->bCaptureOnMovement = false;
			}/**/
			//End Original Code

			UE_LOG(LogTemp, Warning, TEXT("I'm trying"));
			//UE_LOG(TempLog, Warning, TEXT("I'm trying"));

			for (auto CurrentlySearching = OtherMirrors.CreateIterator(); CurrentlySearching; ++CurrentlySearching)
			{
				FMirrorProjectionInfo* SearchingInfo = *CurrentlySearching;
				AMirror* SearchingMirror = SearchingInfo->Mirror;
				ASceneCapture2D* SearchingSceneCapture = SearchingMirror->GetSceneCapture2DActor();
				USceneCaptureComponent2D* SearchingCaptureComponent = SearchingSceneCapture->GetCaptureComponent2D();
				APlanarReflection* SearchingPlanarReflection = SearchingMirror->GetPlanarReflectionActor();
				UPlanarReflectionComponent* SearchingReflectionComponent = SearchingPlanarReflection->GetPlanarReflectionComponent();

				if (IsProjectedFrom(CurrentInfo, SearchingInfo))
				{
					ProjectOnto(CurrentInfo, SearchingInfo);
					NextLevel.Add(SearchingInfo);
					
					//Original Code
					/**/
					SearchingCaptureComponent->bCaptureEveryFrame = true;
					SearchingCaptureComponent->bCaptureOnMovement = true;
					SearchingCaptureComponent->CaptureSortPriority = sortPriority;

					SearchingMirror->UseTextureMaterial();

					SearchingReflectionComponent->bCaptureEveryFrame = false;
					SearchingReflectionComponent->bCaptureOnMovement = false;
					/**/
					//End Original Code
					UE_LOG(LogTemp, Warning, TEXT("%s is a capture mirror"), *SearchingMirror->GetName());
				}
			}
		}

		OtherMirrors.RemoveAll([NextLevel](FMirrorProjectionInfo* next) { return NextLevel.Contains(next); });
		sortPriority++;
	}

	for (auto CurrentProjectionInfo = OtherMirrors.CreateIterator(); CurrentProjectionInfo; ++CurrentProjectionInfo)
	{
		FMirrorProjectionInfo* CurrentInfo = *CurrentProjectionInfo;
		AMirror* CurrentMirror = CurrentInfo->Mirror;
		ASceneCapture2D* CurrentSceneCapture = CurrentMirror->GetSceneCapture2DActor();
		APlanarReflection* CurrentPlanarReflection = CurrentMirror->GetPlanarReflectionActor();
		USceneCaptureComponent2D* CurrentCaptureComponent = CurrentSceneCapture->GetCaptureComponent2D();
		UPlanarReflectionComponent* CurrentReflectionComponent = CurrentPlanarReflection->GetPlanarReflectionComponent();

		CurrentCaptureComponent->bCaptureEveryFrame = false;
		CurrentCaptureComponent->bCaptureOnMovement = false;

		CurrentReflectionComponent->bCaptureEveryFrame = false;
		CurrentReflectionComponent->bCaptureOnMovement = false;

		UE_LOG(LogTemp, Warning, TEXT("%s is not being rendered"), *CurrentMirror->GetName());
	}
}

void AMirrorManager::GetRawRenderingLists(const float DeltaTime, TArray<FMirrorProjectionInfo*>& BaseMirrors, TArray<FMirrorProjectionInfo*>& OtherMirrors)
{
	//APlayerController* Controller = (APlayerController*)Player->Controller;
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	TArray<FMirrorProjectionInfo*> MirrorsLeft = TArray<FMirrorProjectionInfo*>(MirrorRenderingInfo); /*MirrorRenderingInfo.FilterByPredicate([this, DeltaTime](FMirrorProjectionInfo* info)
	{ 
		bool renderedRecently = info->Mirror->WasRecentlyRendered(DeltaTime);
		FString message = renderedRecently ? TEXT("True") : TEXT("False");

		UE_LOG(LogTemp, Warning, TEXT("Was recently rendered: %s"), *message);

		return renderedRecently;
	});*/

	BaseMirrors = MirrorRenderingInfo.FilterByPredicate([this, Controller](FMirrorProjectionInfo* mirrorInfo) { return IsProjectedFromPlayer(Controller, mirrorInfo->Mirror); });
	MirrorsLeft.RemoveAll([BaseMirrors](FMirrorProjectionInfo* mirrorInfo) { return BaseMirrors.Contains(mirrorInfo); });

	OtherMirrors = TArray<FMirrorProjectionInfo*>(MirrorsLeft);
}

void AMirrorManager::UpdateMirrorProjection()
{
	TArray<AActor*> Mirrors = BeingFlipped.FilterByPredicate([](AActor* actor) { return actor->IsA<AMirror>(); });

	for (auto CurrentActor = Mirrors.CreateIterator(); CurrentActor; ++CurrentActor)
	{
		AMirror* CurrentMirror = (AMirror*)*CurrentActor;
		ASceneCapture2D* CurrentSceneCapture = CurrentMirror->GetSceneCapture2DActor();
		USceneCaptureComponent2D* CurrentCaptureComponent = CurrentSceneCapture->GetCaptureComponent2D();
		
		FMatrix ProjectionMatrix;
		BuildProjectionMatrix(CurrentSceneCapture, ProjectionMatrix);
		//Test Stuff Here
		//CurrentCaptureComponent->bUseCustomProjectionMatrix = true;
		//CurrentCaptureComponent->CustomProjectionMatrix = ProjectionMatrix;
		//End Test Stuff

		int32 screenWidth = CurrentCaptureComponent->TextureTarget->GetSurfaceWidth();
		int32 screenHeight = CurrentCaptureComponent->TextureTarget->GetSurfaceHeight();
		FIntRect ViewScreen = FIntRect(0, 0, screenWidth, screenHeight);

		FMirrorProjectionInfo** FoundProjectionInfo = MirrorRenderingInfo.FindByPredicate([CurrentMirror](FMirrorProjectionInfo* info) { return info->Mirror == CurrentMirror; });
		FMirrorProjectionInfo* CurrentProjectionInfo = FoundProjectionInfo ? *FoundProjectionInfo : nullptr;

		if (!CurrentProjectionInfo)
		{
			CurrentProjectionInfo = new FMirrorProjectionInfo(CurrentMirror);
			MirrorRenderingInfo.Add(CurrentProjectionInfo);
		}

		CurrentProjectionInfo->ViewProjectionMatrix = ProjectionMatrix;
		CurrentProjectionInfo->ViewScreen = ViewScreen;
	}
}

bool AMirrorManager::IsProjectedFrom(const FMirrorProjectionInfo* Base, const FMirrorProjectionInfo* Other)
{
	USceneCaptureComponent2D* CaptureComponent = Base->Mirror->GetSceneCapture2DActor()->GetCaptureComponent2D();
	FMatrix ViewProjectionMatrix = Base->ViewProjectionMatrix;
	FIntRect ViewScreen = Base->ViewScreen;
	FVector BaseRightVector = Base->Mirror->GetActorRightVector();

	AMirror* ProjectingTo = Other->Mirror;
	FVector ProjectingRightVector = ProjectingTo->GetActorRightVector();

	//FIntRect screenBounds2 = FIntRect(ViewLocation.X - screenWidth / 2, ViewLocation.Y - screenHeight / 2, ViewLocation.X + screenWidth / 2, ViewLocation.Y + screenHeight / 2);
	//UE_LOG(LogTemp, Warning, TEXT("screen width: %d screen height: %d"), screenWidth, screenHeight);

	FVector2D screenLocation;
	bool projected = ProjectWorldToScreen(ViewProjectionMatrix, ViewScreen, ProjectingTo->GetActorLocation(), screenLocation, true);
	int32 screenX = (int32)screenLocation.X;
	int32 screenY = (int32)screenLocation.Y;
	int32 screenWidth = ViewScreen.Width();
	int32 screenHeight = ViewScreen.Height();
	//UE_LOG(LogTemp, Warning, TEXT("screen x: %d screen y: %d"), screenX, screenY);

	FVector origin, extents;
	ProjectingTo->GetActorBounds(false, origin, extents);

	float dotResult = FVector::DotProduct(BaseRightVector, ProjectingRightVector);
	bool isFacing = dotResult >= 0.0f;

	//FString isFacingMessage = isFacing ? TEXT("true") : TEXT("false");
	//UE_LOG(LogTemp, Warning, TEXT("Is Facing: %s"), *isFacingMessage)

	bool isOnScreen = projected && screenX + extents.X >= 0 && screenY + extents.Y >= 0 && screenX - extents.X < screenWidth && screenY - extents.Y < screenHeight;// && isFacing;

	//FString onscreen = isOnScreen ? TEXT("true") : TEXT("false");
	//UE_LOG(LogTemp, Warning, TEXT("Is on screen: %s"), *onscreen);
	
	return isOnScreen;
}

bool AMirrorManager::IsProjectedFromPlayer(const APlayerController* Controller, const AMirror* Mirror)
{
	int32 screenWidth;
	int32 screenHeight;
	FVector2D screenLocation;
	FVector mirrorLocation = Mirror->GetActorLocation();

	bool projected = Controller->ProjectWorldLocationToScreen(mirrorLocation, screenLocation, true);
	int32 screenX = (int32)screenLocation.X;
	int32 screenY = (int32)screenLocation.Y;

	Controller->GetViewportSize(screenWidth, screenHeight);

	FVector origin, extents;
	Mirror->GetActorBounds(false, origin, extents);

	//float dotResult = FVector::DotProduct(Player->GetActorRightVector(), Mirror->GetActorRightVector());
	//bool isFacing = dotResult >= 0.0f;

	bool isOnScreen = projected && screenX + extents.X >= 0 && screenY + extents.Y >= 0 && screenX - extents.X < screenWidth && screenY - extents.Y < screenHeight;// && isFacing;

	//FString onscreen = isOnScreen ? TEXT("true") : TEXT("false");
	//UE_LOG(LogTemp, Warning, TEXT("Is on screen: %s"), *onscreen);

	return isOnScreen;
}

//void AMirrorManager::ProjectOnto(const FMirrorRenderStack& Base, FMirrorRenderStack& Other)
//{
//	AMirror* BaseMirror = Base.CurrentMirror->Mirror;
//	AMirror* OtherMirror = Other.CurrentMirror->Mirror;
//	ASceneCapture2D* BaseSceneCapture = BaseMirror->GetSceneCapture2DActor();
//	ASceneCapture2D* OtherSceneCapture = OtherMirror->GetSceneCapture2DActor();
//
//	ProjectOnto(BaseSceneCapture, OtherMirror, OtherSceneCapture);
//	Other.InVector = BaseSceneCapture->GetActorRightVector();
//}

void AMirrorManager::ProjectOnto(const FMirrorProjectionInfo* Base, FMirrorProjectionInfo* Other)
{
	AMirror* BaseMirror = Base->Mirror;
	AMirror* OtherMirror = Other->Mirror;
	ASceneCapture2D* BaseSceneCapture = BaseMirror->GetSceneCapture2DActor();
	ASceneCapture2D* OtherSceneCapture = OtherMirror->GetSceneCapture2DActor();

	FVector testnormal = OtherMirror->GetActorLocation() - BaseMirror->GetActorLocation();
	testnormal.Normalize();
	//ProjectOnto(BaseSceneCapture, OtherMirror, OtherSceneCapture);
	ProjectOnto(testnormal/*BaseSceneCapture->GetActorForwardVector()*/, OtherMirror->GetActorUpVector(), OtherSceneCapture);
}

//void AMirrorManager::ProjectOnto(const AActor* Base, const AActor* LookReflector, AActor* Other)
//{
//	/*FVector BaseRight = Base->GetActorRightVector();
//	FVector ReflectingOff = LookReflector->GetActorForwardVector();
//	FVector ReflectedNormal = BaseRight.MirrorByVector(ReflectingOff);
//
//	FQuat rotator = FQuat::FindBetweenNormals(BaseRight, ReflectedNormal);
//	FQuat currentRotation = Other->GetActorQuat();
//	FQuat newRotation = currentRotation * rotator;
//	Other->SetActorRotation(newRotation);*/
//
//	FVector BaseRight = Base->GetActorRightVector();
//	FVector ReflectingOff = LookReflector->GetActorForwardVector();
//	FVector ReflectedNormal = BaseRight.MirrorByVector(ReflectingOff);
//
//	FQuat rotator = FQuat::FindBetweenNormals(BaseRight, ReflectedNormal);
//	FQuat currentRotation = Other->GetActorQuat();
//	FQuat newRotation = currentRotation * rotator;
//	Other->SetActorRotation(newRotation);
//
//
//	FVector displayVector = newRotation.Euler();
//	UE_LOG(LogTemp, Warning, TEXT("X: %f Y: %f Z: %f"), displayVector.X, displayVector.Y, displayVector.Z);
//}

void AMirrorManager::ProjectOnto(const FVector& In, const FVector& LookReflector, AActor* Other)
{
	AActor* Parent = Other->GetAttachParentActor();
	
	FVector CurrentLookDirection = Other->GetActorForwardVector();
	FVector ReflectedNormal = In.MirrorByVector(LookReflector);
	
	if (!CurrentLookDirection.Equals(ReflectedNormal))
	{
		FQuat rotator = FQuat::FindBetweenNormals(CurrentLookDirection, ReflectedNormal);
		//rotator = Other->GetActorTransform().InverseTransformRotation(rotator);
		//FVector test = rotator.Euler();
		//test = FVector(test.Z, test.Y, test.X);
		//rotator = FQuat::MakeFromEuler(test);
		//Other->AddActorWorldRotation(rotator);
		//Other->AddActorLocalRotation(rotator);
		//rotator = Parent->GetActorTransform().InverseTransformRotation(rotator);
		//rotator = Other->GetActorTransform().InverseTransformRotation(rotator);
		rotator = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, rotator.Euler().Z));
		FQuat rotated = Other->ActorToWorld().GetRotation() * rotator;
		//FQuat rotated = rotator * Other->ActorToWorld().GetRotation();
		Other->SetActorRotation(rotated);

		//FQuat currentRotation = Other->GetActorQuat();
		//FQuat newRotation = currentRotation * rotator;
		//Other->SetActorRotation(newRotation);

		FVector displayVector1 = rotator.Euler();
		FVector displayVector2 = Parent->GetActorTransform().TransformRotation(rotator).Euler();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *displayVector1.ToString());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *displayVector2.ToString());
	}
}

void AMirrorManager::BuildProjectionMatrix(const ASceneCapture2D* SceneCapture, FMatrix& ViewProjectionMatrix)
{
	USceneCaptureComponent2D* CaptureComponent = SceneCapture->GetCaptureComponent2D();
	
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

		//Original code
		if (CaptureComponent->bUseCustomProjectionMatrix)
		{
			ProjectionMatrix = CaptureComponent->CustomProjectionMatrix;

			UE_LOG(LogTemp, Warning, TEXT("Probably shouldn't be using custom projection. . . Might want to check your SceneCapture2Ds"));
		}
		else
		{
			BuildProjectionMatrix(CaptureSize, CaptureComponent->ProjectionType, FOV, CaptureComponent->OrthoWidth, ProjectionMatrix);
		}
		
		ViewProjectionMatrix = FTranslationMatrix(-ViewLocation) * ViewRotationMatrix * ProjectionMatrix;
		//End original code

		//Test code
		/*BuildProjectionMatrix(CaptureSize, CaptureComponent->ProjectionType, FOV, CaptureComponent->OrthoWidth, ProjectionMatrix);
		FRotationMatrix ViewRotation(FRotator(90, -180, 0));
		ViewProjectionMatrix = FTranslationMatrix(-ViewLocation) * ViewRotationMatrix * ProjectionMatrix;*/
		//End Test code
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Unable to get projection matrix, TextureTarget is null"));
	}
}

void AMirrorManager::BuildProjectionMatrix(const FIntPoint RenderTargetSize, const ECameraProjectionMode::Type ProjectionType, const float FOV, const float InOrthoWidth, FMatrix& ProjectionMatrix)
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

bool AMirrorManager::ProjectWorldToScreen(const FMatrix& ViewProjectionMatrix, const FIntRect& ViewRect, const FVector& WorldPosition, FVector2D& ScreenPosition, const bool bPlayerViewportRelative)
{
	bool bResult = FSceneView::ProjectWorldToScreen(WorldPosition, ViewRect, ViewProjectionMatrix, ScreenPosition);

	/*if (bPlayerViewportRelative)
	{
		ScreenPosition -= FVector2D(ProjectionData.GetConstrainedViewRect().Min);
	}*/

	//bResult = bResult && Player->PostProcessWorldToScreen(WorldPosition, ScreenPosition, bPlayerViewportRelative);

	return bResult;
}


//void BuildReflectionMatrix(const FVector4 Plane)
//{
//
//}
//
//FVector4 BuildSpacePlane(const FMatrix& WorldToActorMatrix, const FVector& Position, const FVector& Normal, const float SideSign)
//{
//
//}
//
//void BuildObliqueMatrix(FMatrix& Projection, const FVector4& ClipPlane)
//{
//
//}

