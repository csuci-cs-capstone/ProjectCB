#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Grabbable.generated.h"

UINTERFACE(MinimalAPI)
class UGrabbable : public UInterface
{
	GENERATED_BODY()

public:

	enum GrabPriority: unsigned char { GOAL_PRIORITY = 0, PLAYER_PRIORITY, BALL_PRIORITY };

};

class PROJECTCB_API IGrabbable
{
	GENERATED_BODY()

public:

	virtual bool isGrabbable() = 0;

	virtual void makeGrabbed() = 0; // TODO combine? (would be more efficient, but less reliable)
	virtual void setGrabbedPosition(FVector position) = 0;
		// could theoretically be done reliably, but would require additional work

	virtual void launchRelease(FVector direction) = 0;
	
	virtual unsigned char getGrabPriority() = 0;
};
