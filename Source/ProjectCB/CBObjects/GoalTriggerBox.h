#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "../CBGeneral/Grabbable.h"
#include "../CBGeneral/GrabbableList.h"
#include "GoalTriggerBox.generated.h"

UCLASS()
class PROJECTCB_API AGoalTriggerBox : public ATriggerBox, public IGrabbable
{
	GENERATED_BODY()

private:

	static const FVector BALL_POSITIONS[];
	static const FVector BALL_OFFSET;

	static const float DIFF_X;
	static const float DIFF_Y;
	static const float DIFF_Z;

	static const short MAX_BALLS_DISPLAYED;

	GrabbableList m_grabbableList;

	FVector m_currentBallOffset;

	FVector getBallPosition(size_t index);

	bool updateBallOffsetOnAdd();
	bool updateBallOffsetOnRemove();
	void updateBallPositions(bool changedLayout, bool added);

protected:

	void BeginPlay() override;

public:

	AGoalTriggerBox();

	UFUNCTION()
		void OnOverlapBegin(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION()
		void OnOverlapEnd(AActor* overlappedActor, AActor* otherActor);

	bool hasGrabbableObject() override;
	IGrabbableObject* getGrabbableObject() override;
	unsigned char getGrabPriority() override;

	UPROPERTY(Replicated)
	int m_ballsInGoal = 0;
	
};
