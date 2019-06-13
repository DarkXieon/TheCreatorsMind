// Fill out your copyright notice in the Description page of Project Settings.

#include "MirroringBehaviour.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Mirror.h"

//TODO: Set all StaticMeshComponents to moveable
//		Potentially make options to not generate overlap events so the mirrors don't screw up

// Sets default values for this component's properties
UMirroringBehaviour::UMirroringBehaviour()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMirroringBehaviour::BeginPlay()
{
	Super::BeginPlay();
}

void UMirroringBehaviour::Initialize()
{
	GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UMirroringBehaviour::OnOverlapBegin);
	GetOwner()->OnActorEndOverlap.AddDynamic(this, &UMirroringBehaviour::OnOverlapEnd);

	TArray<UActorComponent*> primitiveComponents = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());

	for (UActorComponent* component : primitiveComponents)
	{
		UPrimitiveComponent* primitiveComponent = (UPrimitiveComponent*)component;

		primitiveComponent->bOwnerNoSee = true;
		DisplayedCollision.Add(primitiveComponent, primitiveComponent->GetCollisionResponseToChannels());
	}
}

void UMirroringBehaviour::WorldHide()
{
	TArray<UActorComponent*> primitiveComponents = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());

	FCollisionResponseContainer CollisionResponse(ECollisionResponse::ECR_Ignore);
	CollisionResponse.SetResponse(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	for (UActorComponent* component : primitiveComponents)
	{
		UPrimitiveComponent* primitiveComponent = (UPrimitiveComponent*)component;

		primitiveComponent->SetCollisionResponseToChannels(CollisionResponse);

		if (component->GetOwner()->IsA<AMirror>()) //change this. . . maybe by making a custom behaviour class and putting that on the mirrors
		{
			((UStaticMeshComponent*)component)->SetGenerateOverlapEvents(false);
		}
		else if (component->IsA<UStaticMeshComponent>())
		{
			((UStaticMeshComponent*)component)->SetGenerateOverlapEvents(true);
		}
	}
}

void UMirroringBehaviour::WorldShow()
{
	TArray<UActorComponent*> primitiveComponents = GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass());

	for (UActorComponent* component : primitiveComponents)
	{
		UPrimitiveComponent* primitiveComponent = (UPrimitiveComponent*)component;

		primitiveComponent->SetCollisionResponseToChannels(DisplayedCollision[primitiveComponent]);

		if (component->GetOwner()->IsA<AMirror>()) //change this. . . maybe by making a custom behaviour class and putting that on the mirrors
		{
			((UStaticMeshComponent*)component)->SetGenerateOverlapEvents(true);
		}
		else if (component->IsA<UStaticMeshComponent>())
		{
			((UStaticMeshComponent*)component)->SetGenerateOverlapEvents(false);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Showing object %s"), *GetOwner()->GetFName().ToString());
	}
}

// Called every frame
void UMirroringBehaviour::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMirroringBehaviour::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OverlappedActor->IsA<AMirror>()) //change this. . . maybe by making a custom behaviour class and putting that on the mirrors
	{
		ReverseCulling();

		UE_LOG(LogTemp, Warning, TEXT("Should have front face culling now"));
	}
}

void UMirroringBehaviour::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OverlappedActor->IsA<AMirror>()) //change this. . . maybe by making a custom behaviour class and putting that on the mirrors
	{
		ReverseCulling();
	}
}

void UMirroringBehaviour::ReverseCulling()
{
	TArray<UActorComponent*> primitiveComponents = GetOwner()->GetComponentsByClass(UStaticMeshComponent::StaticClass());

	for (UActorComponent* component : primitiveComponents)
	{
		UStaticMeshComponent* staticMeshComponent = (UStaticMeshComponent*)component;

		staticMeshComponent->SetReverseCulling(!staticMeshComponent->bReverseCulling);
	}
}