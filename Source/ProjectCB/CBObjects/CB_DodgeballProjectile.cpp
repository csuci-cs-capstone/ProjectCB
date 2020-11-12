#include "CB_DodgeballProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

//const FVector ACB_DodgeballProjectile::GOAL_CENTER = FVector();

const float ACB_DodgeballProjectile::PROJECTILE_SPEED = 3000.0f;
const float ACB_DodgeballProjectile::PROJECTILE_GRAVITY = 1.0f;

const float ACB_DodgeballProjectile::GROUND_DECELERATION = 10.0f;

// Sets default values
ACB_DodgeballProjectile::ACB_DodgeballProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->DodgeballMesh = CreateDefaultSubobject<UStaticMeshComponent>("DodgeballMesh");
	//Set to simulate physics so that the projectile is not a kinematic body but a rigid body
	this->DodgeballMesh->SetSimulatePhysics(true);

	SetRootComponent(DodgeballMesh);
	

	this->DodgeballMovement = CreateDefaultSubobject<UProjectileMovementComponent>("DodgeballMovement");
	this->DodgeballMovement->InitialSpeed = 0;
	this->DodgeballMovement->MaxSpeed = ACB_DodgeballProjectile::PROJECTILE_SPEED;
	this->DodgeballMovement->ProjectileGravityScale = ACB_DodgeballProjectile::PROJECTILE_GRAVITY;
	this->DodgeballMovement->bShouldBounce = true;

	this->m_previousVelocity = FVector(0.0f, 0.0f, 0.0f);
	this->m_grounded = true;
	this->m_inGoal = false;

	this->OnActorHit.AddDynamic(this, &ACB_DodgeballProjectile::OnHit);
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

	FVector velocity = this->DodgeballMesh->GetPhysicsLinearVelocity();

	if (this->m_inGoal)
	{
		if (this->isGrabbable())
		{
			this->DodgeballMesh->SetSimulatePhysics(false);

			this->SetActorLocation(this->m_goalLocation); // TODO make move towards position from it's current position
		}
	}
	else
	{
		if (this->m_grounded)
		{
			if (velocity.X > ACB_DodgeballProjectile::GROUND_DECELERATION)
				velocity.X = velocity.X - ACB_DodgeballProjectile::GROUND_DECELERATION;
			else if (velocity.X < -ACB_DodgeballProjectile::GROUND_DECELERATION)
				velocity.X = velocity.X + ACB_DodgeballProjectile::GROUND_DECELERATION;
			else
				velocity.X = 0.0f;

			if (velocity.Y > ACB_DodgeballProjectile::GROUND_DECELERATION)
				velocity.Y = velocity.Y - ACB_DodgeballProjectile::GROUND_DECELERATION;
			else if (velocity.Y < -ACB_DodgeballProjectile::GROUND_DECELERATION)
				velocity.Y = velocity.Y + ACB_DodgeballProjectile::GROUND_DECELERATION;
			else
				velocity.Y = 0.0f;

			this->DodgeballMesh->SetPhysicsLinearVelocity(velocity);
			
		}
		else if (velocity.Z >= 0 && this->m_previousVelocity.Z <= 0)
			this->m_grounded = true;
	}

	this->m_previousVelocity = velocity;
}

ACB_DodgeballProjectile::BallState ACB_DodgeballProjectile::getBallState()
{
	return this->m_ballState;
}

void ACB_DodgeballProjectile::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("We got a hit!"));

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("OnHit is called!"));
}

bool ACB_DodgeballProjectile::isGrabbable()
{
	return this->getBallState() == ACB_DodgeballProjectile::BALL_PROJECTILE;
}

void ACB_DodgeballProjectile::makeGrabbed()
{
	this->m_ballState = ACB_DodgeballProjectile::BALL_GRABBED;

	this->DodgeballMesh->SetSimulatePhysics(false);
}

void ACB_DodgeballProjectile::launchRelease(FVector direction)
{
	this->m_ballState = ACB_DodgeballProjectile::BALL_PROJECTILE;

	this->m_grounded = false;

	this->DodgeballMesh->SetSimulatePhysics(false);

	direction.Normalize();

	//this->DodgeballMesh->SetPhysicsLinearVelocity(ACB_DodgeballProjectile::PROJECTILE_SPEED * direction);
	this->DodgeballMovement->SetVelocityInLocalSpace(ACB_DodgeballProjectile::PROJECTILE_SPEED * direction);
}

void ACB_DodgeballProjectile::setGrabbed(FVector position, FRotator rotation)
{
	this->SetActorLocation(position);
	this->SetActorRotation(rotation);
}

bool ACB_DodgeballProjectile::hasGrabbableObject()
{
	return this->isGrabbable();
}

IGrabbableObject* ACB_DodgeballProjectile::getGrabbableObject()
{
	return this;
}

unsigned char ACB_DodgeballProjectile::getGrabPriority()
{
	return UGrabbable::BALL_PRIORITY;
}