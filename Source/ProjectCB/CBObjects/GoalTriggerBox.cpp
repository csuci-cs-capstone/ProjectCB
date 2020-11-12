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

FVector AGoalTriggerBox::getBallPosition(size_t index)
{
	return this->GetActorLocation() + this->GetActorRotation().RotateVector(this->m_currentBallOffset
		+ AGoalTriggerBox::BALL_POSITIONS[index % AGoalTriggerBox::MAX_BALLS_DISPLAYED]);
}

void AGoalTriggerBox::updateBallOffset()
{
	switch (this->m_grabbableList.length())
	{
	case 2:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 3:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 4:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 11:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 12:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 13:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 14:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 15:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 16:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 17:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 18:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 19:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	case 20:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET + FVector(0.0f, -AGoalTriggerBox::DIFF_Y / 2.0f, 0.0f);
		break;
	default:
		this->m_currentBallOffset = AGoalTriggerBox::BALL_OFFSET;
		break;
	}
}

void AGoalTriggerBox::updateBallPositions()
{
	AGoalTriggerBox::updateBallOffset();

	for (size_t index = 0; index < this->m_grabbableList.length(); index++)
	{
		ACB_DodgeballProjectile* dodgeball = Cast<ACB_DodgeballProjectile>(this->m_grabbableList[index]);

		dodgeball->m_goalLocation = this->getBallPosition(index);
	}
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

			this->m_grabbableList.add(dodgeball);

			updateBallPositions();
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

			dodgeball->m_inGoal = false;

			this->m_grabbableList.remove(dodgeball);

			updateBallPositions();
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