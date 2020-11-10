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
}

void AGoalTriggerBox::OnOverlapBegin(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (GEngine) // TODO remove
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("ENTERED")));

		if (otherActor->IsA(ACB_DodgeballProjectile::StaticClass()))
		{
			// add ball to ball arraylist
			// make ball unable to move and roll towards center?
		}
		else if (otherActor->IsA(ACB_PlayerCharacter::StaticClass()))
		{
			// allow for ball to be picked up
					// if grab is pressed, ball is picked up and removed from top of ball list if is any
		}
	}
}

void AGoalTriggerBox::OnOverlapEnd(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (GEngine) // TODO remove
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("EXITED")));

		// TODO make it so that...

			// if otherActor is ball
				// do nothing

			// else if player
				// make unable to grab?
	}
}