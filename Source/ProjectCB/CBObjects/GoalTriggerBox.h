#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "GoalTriggerBox.generated.h"

UCLASS()
class PROJECTCB_API AGoalTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;

public:

	AGoalTriggerBox();

	UFUNCTION()
		void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION()
		void OnOverlapEnd(AActor* overlappedActor, AActor* otherActor);
	
};
