#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
//#include "../CBGeneral/Grabbable.h"
#include "GoalTriggerBox.generated.h"

UCLASS()
class PROJECTCB_API AGoalTriggerBox : public ATriggerBox//, public IGrabbable
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

	//bool isGrabbable() override;
	//void makeGrabbed() override;
	//void launchRelease(FVector direction) override;
	//void setGrabbedPosition(FVector position) override;
	
};
