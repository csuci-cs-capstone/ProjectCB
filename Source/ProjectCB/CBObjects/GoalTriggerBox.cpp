#include "GoalTriggerBox.h"
#include "DrawDebugHelpers.h" // TODO remove
#include "CB_DodgeballProjectile.h"
#include <math.h>
#include "../CBPlayer/CB_PlayerCharacter.h"

const float AGoalTriggerBox::DIFF_Y = 50.0f;
const float AGoalTriggerBox::DIFF_X = -sqrt(3.0f) * DIFF_Y / 2.0f;
const float AGoalTriggerBox::DIFF_Z = 1.5f * DIFF_Y / 2.0f;

const short AGoalTriggerBox::MAX_BALLS_DISPLAYED = 35;

const FVector AGoalTriggerBox::BALL_OFFSET
	= FVector(100.0f - (AGoalTriggerBox::DIFF_Y / 2), 0.0f, -100.0f - AGoalTriggerBox::DIFF_Y);

const FVector AGoalTriggerBox::BALL_POSITIONS[] = {

	// STACK 1

	FVector(0.0f, 0.0f, 0.0f),

	// STACK 2

	FVector(0.0f, DIFF_Y, 0.0f),
	FVector(DIFF_X, 0.5f * DIFF_Y, 0.0f),

	FVector(0.5f * DIFF_X, 0.5f * DIFF_Y, DIFF_Z),

	// STACK 3

	FVector(0.0f, -DIFF_Y, 0.0f),
	FVector(DIFF_X, 0.5f * -DIFF_Y, 0.0f),
	FVector(2.0f * DIFF_X, 0.0f, 0.0f),

	FVector(0.5f * DIFF_X, 0.5f * -DIFF_Y, DIFF_Z),
	FVector(1.5f * DIFF_X, 0.0f, DIFF_Z),

	FVector(1.0f * DIFF_X, 0.0f, 2.0f * DIFF_Z),

	// STACK 4

	FVector(0.0f, 2.0f * DIFF_Y, 0.0f),
	FVector(DIFF_X, 1.5f * DIFF_Y, 0.0f),
	FVector(2.0f * DIFF_X, DIFF_Y, 0.0f),
	FVector(3.0f * DIFF_X, 0.5f * DIFF_Y, 0.0f),

	FVector(0.5f * DIFF_X, 1.5f * DIFF_Y, DIFF_Z),
	FVector(1.5f * DIFF_X, DIFF_Y, DIFF_Z),
	FVector(2.5f * DIFF_X, 0.5f * DIFF_Y, DIFF_Z),

	FVector(1.0f * DIFF_X, DIFF_Y, 2.0f * DIFF_Z),
	FVector(2.0f * DIFF_X, 0.5f * DIFF_Y, 2.0f * DIFF_Z),

	FVector(1.5f * DIFF_X, 0.5f * DIFF_Y, 3.0f * DIFF_Z),

	// STACK 5

	FVector(0.0f, 2.0f * -DIFF_Y, 0.0f),
	FVector(DIFF_X, 1.5f * -DIFF_Y, 0.0f),
	FVector(2.0f * DIFF_X, -DIFF_Y, 0.0f),
	FVector(3.0f * DIFF_X, 0.5f * -DIFF_Y, 0.0f),
	FVector(4.0f * DIFF_X, 0.0f, 0.0f),

	FVector(0.5f * DIFF_X, 1.5f * -DIFF_Y, DIFF_Z),
	FVector(1.5f * DIFF_X, -DIFF_Y, DIFF_Z),
	FVector(2.5f * DIFF_X, 0.5f * -DIFF_Y, DIFF_Z),
	FVector(3.5f * DIFF_X, 0.0f, DIFF_Z),

	FVector(1.0f * DIFF_X, -DIFF_Y, 2.0f * DIFF_Z),
	FVector(2.0f * DIFF_X, 0.5f * -DIFF_Y, 2.0f * DIFF_Z),
	FVector(3.0f * DIFF_X, 0.0f, 2.0f * DIFF_Z),

	FVector(1.5f * DIFF_X, 0.5f * -DIFF_Y, 3.0f * DIFF_Z),
	FVector(2.5f * DIFF_X, 0.0f, 3.0f * DIFF_Z),

	FVector(2.0f * DIFF_X, 0.0f, 4.0f * DIFF_Z),
};

FVector AGoalTriggerBox::getBallOffset()
{
	//switch (this->m_grabbableList.length())
	//{
	//case 0:
	//	return AGoalTriggerBox::BALL_OFFSET;
	//case 1:
	//	return AGoalTriggerBox::BALL_OFFSET;
	//default:
	//	return AGoalTriggerBox::BALL_OFFSET;
	//}
	return AGoalTriggerBox::BALL_OFFSET;
}

AGoalTriggerBox::AGoalTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &AGoalTriggerBox::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &AGoalTriggerBox::OnOverlapEnd);
}

void AGoalTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetComponentsBoundingBox().GetExtent(), FColor::Green, true, -1, 0, 5);
}

void AGoalTriggerBox::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (otherActor->IsA(ACB_DodgeballProjectile::StaticClass()))
		{
			ACB_DodgeballProjectile* dodgeball = Cast<ACB_DodgeballProjectile>(otherActor);

			dodgeball->m_inGoal = true;

			dodgeball->m_goalLocation = this->GetActorLocation() + this->GetActorRotation().RotateVector(
				AGoalTriggerBox::BALL_OFFSET
				+ AGoalTriggerBox::BALL_POSITIONS[this->m_grabbableList.length() % AGoalTriggerBox::MAX_BALLS_DISPLAYED]);

			// TODO update position of all dodgeballs based on offset for all additions / subtractions upon change in length

			this->m_grabbableList.add(dodgeball);
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