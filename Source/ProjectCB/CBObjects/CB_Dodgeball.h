// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CB_Dodgeball.generated.h"

UCLASS()
class PROJECTCB_API ACB_Dodgeball : public AActor
{
	GENERATED_BODY()

private:

	//UPROPERTY()
	//	class UParticleSystemComponent* m_particleSystem;

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* m_mesh;

	//UPROPERTY(VisibleAnywhere)
	//	class USphereComponent* m_collider;

	// Sphere collision component.
	//UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	//	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* m_collider;

	FVector m_velocity;
	const float m_speed = 2000.0f;
	const float m_gravity = 24.0f;

public:	
	// Sets default values for this actor's properties
	ACB_Dodgeball();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function that initializes the projectile's velocity in the shoot direction.
	void FireInDirection(const FVector& ShootDirection);
};
