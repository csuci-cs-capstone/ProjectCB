// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_DodgeballProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ACB_DodgeballProjectile::ACB_DodgeballProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->DodgeballMesh = CreateDefaultSubobject<UStaticMeshComponent>("DodgeballMesh");

	SetRootComponent(DodgeballMesh);

	this->DodgeballMovement = CreateDefaultSubobject<UProjectileMovementComponent>("DodgeballMovement");
	this->DodgeballMovement->InitialSpeed = 0;
	this->DodgeballMovement->MaxSpeed = this->m_speed;
	this->DodgeballMovement->ProjectileGravityScale = this->m_gravity;
	this->DodgeballMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void ACB_DodgeballProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACB_DodgeballProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACB_DodgeballProjectile::launch(const FVector& direction)
{
	this->DodgeballMovement->AddForce(this->m_speed * 1000 * direction);
}

