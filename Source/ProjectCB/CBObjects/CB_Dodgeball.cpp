// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_Dodgeball.h"

// Sets default values
ACB_Dodgeball::ACB_Dodgeball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->m_mesh = CreateDefaultSubobject<UStaticMeshComponent>("Dodgeball");
}

// Called when the game starts or when spawned
void ACB_Dodgeball::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACB_Dodgeball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector nextLocation = GetActorLocation();

	nextLocation.Y += this->m_speed;

	SetActorLocation(nextLocation);
}

