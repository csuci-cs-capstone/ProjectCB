#include "CB_DodgeballProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "../CBPlayer/CB_PlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

//const FVector ACB_DodgeballProjectile::GOAL_CENTER = FVector();

const float ACB_DodgeballProjectile::BALL_RADIUS = 25.0f;
const float ACB_DodgeballProjectile::PROJECTILE_SPEED = 3000.0f;
const float ACB_DodgeballProjectile::PROJECTILE_GRAVITY = 1.0f;

const float ACB_DodgeballProjectile::GROUND_DECELERATION = 10.0f;

// Sets default values
ACB_DodgeballProjectile::ACB_DodgeballProjectile()
{
	this->bReplicates = true;
	this->SetReplicateMovement(true);
	this->bNetLoadOnClient = true;
	

 	this->m_ballState = ACB_DodgeballProjectile::BALL_GROUNDED;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->DodgeballMesh = CreateDefaultSubobject<UStaticMeshComponent>("DodgeballMesh");
	this->DodgeballMesh->SetIsReplicated(true);
	//Set to simulate physics so that the projectile is not a kinematic body but a rigid body
	this->DodgeballMesh->SetSimulatePhysics(true);
	this->DodgeballMesh->SetShouldUpdatePhysicsVolume(false);


	SetRootComponent(DodgeballMesh);

	this->DodgeballMovement = CreateDefaultSubobject<UProjectileMovementComponent>("DodgeballMovement");
	this->DodgeballMovement->InitialSpeed = 0;
	this->DodgeballMovement->MaxSpeed = ACB_DodgeballProjectile::PROJECTILE_SPEED;
	this->DodgeballMovement->ProjectileGravityScale = ACB_DodgeballProjectile::PROJECTILE_GRAVITY;
	this->DodgeballMovement->bShouldBounce = true;
	this->DodgeballMovement->SetIsReplicated(true);

	this->m_previousVelocity = FVector(0.0f, 0.0f, 0.0f);
	this->m_inGoal = false;

	this->OnActorHit.AddDynamic(this, &ACB_DodgeballProjectile::OnHit);
}

// Called when the game starts or when spawned
void ACB_DodgeballProjectile::BeginPlay()
{
	Super::BeginPlay();
	if(this->m_playerRef != nullptr)
		this->DodgeballMesh->IgnoreActorWhenMoving(this->m_playerRef, true);
	this->m_resetCollisionFrame = FPlayerBasics::RESET_COLLISION_FRAMES + 1;
}

// Called every frame
void ACB_DodgeballProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->GetActorLocation().Z < -500.0f)
		this->Destroy();

	FVector velocity = this->DodgeballMesh->GetPhysicsLinearVelocity();

	if (this->m_inGoal)
	{
		if (this->isGrabbable())
		{
			this->DodgeballMesh->SetSimulatePhysics(false);

			this->SetActorLocation(this->m_goalLocation);
		}
	}
	else
	{
		switch (this->m_ballState)
		{
		case BALL_GROUNDED:
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
			{
				this->DodgeballMesh->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 0.0f));
				velocity.Y = 0.0f;
			}

			this->DodgeballMesh->SetPhysicsLinearVelocity(velocity);
			break;
		case BALL_PROJECTILE:
			if (this->GetActorLocation().Z < 125.01f)
			{
				this->m_ballState = ACB_DodgeballProjectile::BALL_GROUNDED;
				this->DodgeballMesh->SetSimulatePhysics(true);
			}
			break;
		default:
			break;
		}
	}

	this->m_previousVelocity = velocity;

	if (this->m_resetCollisionFrame >= 0)
	{
		if (this->m_resetCollisionFrame < FPlayerBasics::RESET_COLLISION_FRAMES)
			this->m_resetCollisionFrame++;
		else if (this->m_resetCollisionFrame == FPlayerBasics::RESET_COLLISION_FRAMES)
		{
			//if (this->m_playerRef != nullptr)
				//this->DodgeballMesh->IgnoreActorWhenMoving(this->m_playerRef, false);
			this->SetActorEnableCollision(true);
			this->m_resetCollisionFrame++;
		}
	}
}

bool ACB_DodgeballProjectile::inAir()
{
	return this->m_ballState == ACB_DodgeballProjectile::BALL_PROJECTILE;
}

float ACB_DodgeballProjectile::getRadius()
{
	return ACB_DodgeballProjectile::BALL_RADIUS;
}

bool ACB_DodgeballProjectile::isGrabbable()
{
	return this->m_ballState == ACB_DodgeballProjectile::BALL_GROUNDED
		|| this->m_ballState == ACB_DodgeballProjectile::BALL_PROJECTILE;
}

void ACB_DodgeballProjectile::makeGrabbed_Implementation()
{
	this->m_ballState = ACB_DodgeballProjectile::BALL_GRABBED;

	this->DodgeballMesh->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 0.0f));
	this->DodgeballMesh->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f));

	this->DodgeballMesh->SetSimulatePhysics(false);
}

void ACB_DodgeballProjectile::launchRelease_Implementation(FVector direction, FRotator rotation)
{
	this->m_ballState = ACB_DodgeballProjectile::BALL_PROJECTILE;

	this->m_resetCollisionFrame = 0;
	this->DodgeballMesh->SetSimulatePhysics(false);
	this->DodgeballMovement->Velocity = ACB_DodgeballProjectile::PROJECTILE_SPEED * direction;
	this->DodgeballMovement->Velocity.Z = 0.0f;

	/*FVector adjustedLocation = GetActorLocation();
	FVector teleportLocation = adjustedLocation;

	this->GetWorld()->FindTeleportSpot(this, teleportLocation, rotation);
	this->SetActorLocationAndRotation(teleportLocation, rotation, false, nullptr, ETeleportType::ResetPhysics);*/
	this->SetActorRotation(rotation);
	
}

bool ACB_DodgeballProjectile::launchRelease_Validate(FVector direction, FRotator rotation)
{
	return true;
}

void ACB_DodgeballProjectile::setGrabbed_Implementation(FVector position, FRotator rotation)
{
	if (this->m_isIgnoreSet == false)
	{
		this->m_resetCollisionFrame = -1;
		this->m_isIgnoreSet = true;
	}
	
	this->SetActorEnableCollision(false);

	this->SetActorLocation(position);
	this->SetActorRotation(rotation);
	this->m_ballState = ACB_DodgeballProjectile::BALL_GRABBED;
	this->DodgeballMesh->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 0.0f));
	this->DodgeballMesh->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f));
	this->DodgeballMesh->SetEnableGravity(false);
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


void ACB_DodgeballProjectile::OnHit(AActor* selfActor, AActor* otherActor, FVector normalImpulse, const FHitResult& hit)
{
	if (this->inAir() && otherActor->IsA(ACB_PlayerCharacter::StaticClass()))
	{
		FVector velocity = this->DodgeballMovement->Velocity;
		velocity.Z = 0.0f;

		ACB_PlayerCharacter* player = Cast<ACB_PlayerCharacter>(otherActor);
		player->m_basics.launchPlayer(velocity, velocity.Rotation());

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("PlayerHit"));
	}

}
