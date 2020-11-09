#include "GoalTriggerBox.h"
#include "DrawDebugHelpers.h"

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
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("ENTERED")));
	}
}

void AGoalTriggerBox::OnOverlapEnd(AActor* overlappedActor, AActor* otherActor)
{
	if (otherActor && otherActor != this)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("EXITED")));
	}
}