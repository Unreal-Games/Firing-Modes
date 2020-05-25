// Fill out your copyright notice in the Description page of Project Settings.


#include "ForceActor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AForceActor::AForceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ForceCube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlayerForce"));
	ForceCube->SetupAttachment(RootComponent);

	ForceStrength = 10000.0f;
}

// Called when the game starts or when spawned
void AForceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AForceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector Up=ForceCube->GetUpVector();
	ForceCube->AddForce(Up * ForceStrength * ForceCube->GetMass());
}

