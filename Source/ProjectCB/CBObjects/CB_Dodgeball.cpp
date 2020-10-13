// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_Dodgeball.h"
#include "Components/SphereComponent.h"

// Sets default values
ACB_Dodgeball::ACB_Dodgeball()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->m_collider = CreateDefaultSubobject<USphereComponent>(TEXT("DodgeballCollider"));
	this->m_collider->InitSphereRadius(25.0f);
	RootComponent = this->m_collider;

	this->m_mesh = CreateDefaultSubobject<UStaticMeshComponent>("DodgeballMesh");
	//SetRootComponent(this->m_mesh);
	this->m_mesh->SetupAttachment(RootComponent);
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

	this->m_velocity = this->m_velocity - FVector(0, 0, this->m_gravity);

	SetActorLocation(GetActorLocation() + (DeltaTime * this->m_velocity));
}

// Function that initializes the projectile's velocity in the shoot direction.
void ACB_Dodgeball::FireInDirection(const FVector& ShootDirection)
{
	this->m_velocity = this->m_speed * ShootDirection;
}

