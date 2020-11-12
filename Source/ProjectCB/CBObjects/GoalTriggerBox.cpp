#include "GoalTriggerBox.h"
#include "DrawDebugHelpers.h" // TODO remove
#include "CB_DodgeballProjectile.h"
#include <math.h>
#include "../CBPlayer/CB_PlayerCharacter.h"

const float AGoalTriggerBox::DIFF_Y = 50.0f;
const float AGoalTriggerBox::DIFF_X = -sqrt(3.0f) * DIFF_Y / 2.0f;
const float AGoalTriggerBox::DIFF_Z = 1.5f * DIFF_Y / 2.0f;

const short AGoalTriggerBox::MAX_BALLS_DISPLAYED = 19; // TODO change to be accurate with game

const FVector AGoalTriggerBox::BALL_OFFSET = FVector(100.0f, 0.0f, -150.0f);

const FVector AGoalTriggerBox::BALL_POSITIONS[] = {

	// LAYER 1

	FVector(0.0f, 0.0f, 0.0f),
	FVector(0.0f, DIFF_Y, 0.0f),
	FVector(0.0f, -DIFF_Y, 0.0f),
	FVector(0.0f, 2.0f * DIFF_Y, 0.0f),
	FVector(0.0f, 2.0f * -DIFF_Y, 0.0f),

	FVector(DIFF_X, 0.5f * DIFF_Y, 0.0f),
	FVector(DIFF_X, 0.5f * -DIFF_Y, 0.0f),
	FVector(DIFF_X, 1.5f * DIFF_Y, 0.0f),
	FVector(DIFF_X, 1.5f * -DIFF_Y, 0.0f),

	FVector(2.0f * DIFF_X, 0.0f, 0.0f),
	FVector(2.0f * DIFF_X, DIFF_Y, 0.0f),
	FVector(2.0f * DIFF_X, -DIFF_Y, 0.0f),

	FVector(3.0f * DIFF_X, 0.5f * DIFF_Y, 0.0f),
	FVector(3.0f * DIFF_X, 0.5f * -DIFF_Y, 0.0f),

	FVector(4.0f * DIFF_X, 0.0f, 0.0f),

	// LAYER 2

	FVector(0.5f * DIFF_X, 0.5f * DIFF_Y, DIFF_Z),
	FVector(0.5f * DIFF_X, 0.5f * -DIFF_Y, DIFF_Z),
	FVector(0.5f * DIFF_X, 1.5f * DIFF_Y, DIFF_Z),
	FVector(0.5f * DIFF_X, 1.5f * -DIFF_Y, DIFF_Z),

	FVector(1.5f * DIFF_X, 0.0f, DIFF_Z),
	FVector(1.5f * DIFF_X, DIFF_Y, DIFF_Z),
	FVector(1.5f * DIFF_X, -DIFF_Y, DIFF_Z),

	FVector(2.5f * DIFF_X, 0.5f * DIFF_Y, DIFF_Z),
	FVector(2.5f * DIFF_X, 0.5f * -DIFF_Y, DIFF_Z),

	FVector(3.5f * DIFF_X, 0.0f, DIFF_Z),
};

AGoalTriggerBox::AGoalTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AGoalTriggerBox::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AGoalTriggerBox::OnOverlapEnd);
}

void AGoalTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Green, true, -1, 0, 5);

	//FVector scale = this->GetActorTransform().GetLocation();

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("size: %d"), scale.X));

	//FVector relativeScale = this->GetTransform().GetScale3D();

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("relative: %d"), relativeScale.X));

	//this->GetActorTransform().GetScale3D();

	// 8.75 4.25
}

void AGoalTriggerBox::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (otherActor->IsA(ACB_DodgeballProjectile::StaticClass()))
		{
			ACB_DodgeballProjectile* dodgeball = Cast<ACB_DodgeballProjectile>(otherActor);

			dodgeball->m_inGoal = true;

			dodgeball->m_goalLocation = this->GetActorLocation() + AGoalTriggerBox::BALL_OFFSET
				+ AGoalTriggerBox::BALL_POSITIONS[this->m_grabbableList.length() % AGoalTriggerBox::MAX_BALLS_DISPLAYED];

			this->m_grabbableList.add(dodgeball);

			

			// TODO make it move towards the position from it's current position
			

			// TODO make ball unable to move and roll towards center?
		}
	}
}

void AGoalTriggerBox::OnOverlapEnd(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (otherActor->IsA(ACB_DodgeballProjectile::StaticClass()))
		{
			ACB_DodgeballProjectile* dodgeball = Cast<ACB_DodgeballProjectile>(otherActor);

			this->m_grabbableList.remove(dodgeball);

			dodgeball->m_inGoal = false;

			// TODO make ball able to move again?
		}
	}
}

bool AGoalTriggerBox::hasGrabbableObject()
{
	return (bool) this->m_grabbableList.getTopGrabbable();
}

IGrabbableObject* AGoalTriggerBox::getGrabbableObject()
{
	IGrabbable* grabbable = this->m_grabbableList.getTopGrabbable();

	return grabbable ? grabbable->getGrabbableObject() : nullptr;
}

unsigned char AGoalTriggerBox::getGrabPriority()
{
	return UGrabbable::GOAL_PRIORITY;
}