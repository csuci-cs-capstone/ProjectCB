#include "GoalTriggerBox.h"
#include "DrawDebugHelpers.h" // TODO remove
#include "CB_DodgeballProjectile.h"
#include "../CBPlayer/CB_PlayerCharacter.h"

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

			this->m_grabbableList.add(dodgeball);

			dodgeball->m_inGoal = true;

			// TODO make it move towards the position from it's current position
			dodgeball->m_goalLocation = this->GetActorLocation();

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