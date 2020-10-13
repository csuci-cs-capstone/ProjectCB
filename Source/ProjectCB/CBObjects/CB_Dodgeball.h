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

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* m_mesh;

	FVector m_velocity;
	float m_speed;

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

	//// Projectile movement component.
	//UPROPERTY(VisibleAnywhere, Category = "Movement")
	//	UProjectileMovementComponent* ProjectileMovementComponent;

};
