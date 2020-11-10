#include "CB_DodgeballProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

const float ACB_DodgeballProjectile::PROJECTILE_SPEED = 2500.0f;
const float ACB_DodgeballProjectile::PROJECTILE_GRAVITY = 1.0f;

// Sets default values
ACB_DodgeballProjectile::ACB_DodgeballProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->DodgeballMesh = CreateDefaultSubobject<UStaticMeshComponent>("DodgeballMesh");

	SetRootComponent(DodgeballMesh);

	this->DodgeballMovement = CreateDefaultSubobject<UProjectileMovementComponent>("DodgeballMovement");
	this->DodgeballMovement->InitialSpeed = 0;
	this->DodgeballMovement->MaxSpeed = ACB_DodgeballProjectile::PROJECTILE_SPEED;
	this->DodgeballMovement->ProjectileGravityScale = ACB_DodgeballProjectile::PROJECTILE_GRAVITY;
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

ACB_DodgeballProjectile::BallState ACB_DodgeballProjectile::getBallState()
{
	return this->m_ballState;
}

bool ACB_DodgeballProjectile::isGrabbable()
{
	return this->getBallState() == ACB_DodgeballProjectile::BALL_PROJECTILE;
}

void ACB_DodgeballProjectile::makeGrabbed()
{
	this->m_ballState = ACB_DodgeballProjectile::BALL_GRABBED;
}

void ACB_DodgeballProjectile::launchRelease(FVector direction)
{
	this->m_ballState = ACB_DodgeballProjectile::BALL_PROJECTILE;

	this->DodgeballMovement->AddForce(ACB_DodgeballProjectile::PROJECTILE_SPEED * 1000 * direction);
}

void ACB_DodgeballProjectile::setGrabbedPosition(FVector position)
{
	this->SetActorLocation(position);
}

unsigned char ACB_DodgeballProjectile::getGrabPriority()
{
	return UGrabbable::BALL_PRIORITY;
}