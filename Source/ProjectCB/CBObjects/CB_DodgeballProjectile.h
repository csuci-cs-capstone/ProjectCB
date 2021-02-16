#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../CBGeneral/Grabbable.h"
#include "../CBGeneral/GrabbableObject.h"
#include "CB_DodgeballProjectile.generated.h"

UCLASS()
class PROJECTCB_API ACB_DodgeballProjectile : public AActor, public IGrabbable, public IGrabbableObject
{
	GENERATED_BODY()

private:

	enum BallState { BALL_GROUNDED = 0, BALL_GRABBED, BALL_PROJECTILE };
	
	static const float BALL_RADIUS;
	static const float PROJECTILE_SPEED;
	static const float PROJECTILE_GRAVITY;

	static const float GROUND_DECELERATION;

	BallState m_ballState;

	FVector m_previousVelocity;

	short m_resetCollisionFrame;

public:

	bool m_inGoal;
	FVector m_goalLocation;

	AActor* m_playerRef;

	// Sets default values for this actor's properties
	ACB_DodgeballProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* DodgeballMesh;

	UPROPERTY(EditAnywhere, Category = "Components")
		class UProjectileMovementComponent* DodgeballMovement;

	UFUNCTION()
		void OnHit(AActor* selfActor, AActor* otherActor, FVector normalImpulse, const FHitResult& hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool inAir();

	// Grabbable

	float getRadius() override;
	bool isGrabbable() override;
	void makeGrabbed() override;
	void launchRelease(FVector direction, FRotator rotation) override;
	void setGrabbed(FVector position, FRotator rotation) override;

	bool hasGrabbableObject() override;
	IGrabbableObject* getGrabbableObject() override;
	unsigned char getGrabPriority() override;
	

};
