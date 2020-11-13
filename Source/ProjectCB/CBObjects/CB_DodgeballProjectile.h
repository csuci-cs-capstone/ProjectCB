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

public:

	enum BallState { BALL_PROJECTILE = 0, BALL_GRABBED };

private:

	//static const FVector GOAL_CENTER;
	
	static const float PROJECTILE_SPEED;
	static const float PROJECTILE_GRAVITY;

	static const float GROUND_DECELERATION;

	BallState m_ballState;

	FVector m_previousVelocity;
	bool m_grounded;

public:

	bool m_inGoal;
	FVector m_goalLocation;

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

	BallState getBallState();

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);


	// Grabbable

	bool isGrabbable() override;
	void makeGrabbed() override;
	void launchRelease(FVector direction, FRotator rotation) override;
	void setGrabbed(FVector position, FRotator rotation) override;

	bool hasGrabbableObject() override;
	IGrabbableObject* getGrabbableObject() override;
	unsigned char getGrabPriority() override;
	
};
