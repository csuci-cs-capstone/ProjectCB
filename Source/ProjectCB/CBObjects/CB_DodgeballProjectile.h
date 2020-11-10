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
	
	static const float PROJECTILE_SPEED;
	static const float PROJECTILE_GRAVITY;

	BallState m_ballState;

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

	BallState getBallState();

	// Grabbable

	bool isGrabbable() override;
	void makeGrabbed() override;
	void launchRelease(FVector direction) override;
	void setGrabbedPosition(FVector position) override;
	unsigned char getGrabPriority() override;
	
};
