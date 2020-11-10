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

	GrabbableList m_grabbableList;

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
	
};
