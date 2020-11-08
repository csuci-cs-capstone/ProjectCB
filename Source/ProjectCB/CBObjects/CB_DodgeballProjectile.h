#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../CBGeneral/Grabbable.h"
#include "CB_DodgeballProjectile.generated.h"

UCLASS()
class PROJECTCB_API ACB_DodgeballProjectile : public AActor, public IGrabbable
{
	GENERATED_BODY()

public:

	enum BallState { BALL_PROJECTILE = 0, BALL_GRABBED };

private:

	BallState m_ballState; // TODO make private?
	
	FVector m_velocity;
	const float m_speed = 2500.0f;
	const float m_gravity = 1.0f;
	const float m_bounce = 5.0f;

public:	

	// Sets default values for this actor's properties
	ACB_DodgeballProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* DodgeballMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
		class UProjectileMovementComponent* DodgeballMovement;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void launch(const FVector& direction);

	BallState getBallState();

	// Grabbable

	bool isGrabbable() override;
	void makeGrabbed() override;
	void makeUngrabbed() override;
	void setGrabbedPosition(FVector position) override;

};
