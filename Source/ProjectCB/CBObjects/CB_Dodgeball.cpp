// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_Dodgeball.h"
//#include "Classes/GameFramework/ProjectileMovementComponent.h"

// Sets default values
ACB_Dodgeball::ACB_Dodgeball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->m_mesh = CreateDefaultSubobject<UStaticMeshComponent>("DodgeballMesh");

	SetRootComponent(this->m_mesh);

	//this->m_movement = CreateDefaultSubobject<UProjectileMovementComponent>("DodgeballMovement");

	//this->m_movement->InitialSpeed = this->m_speed;
	//this->m_movement->MaxSpeed = this->m_speed;
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

