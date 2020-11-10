#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrabbableObject.h"
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

	virtual bool hasGrabbableObject() = 0;

	virtual IGrabbableObject* getGrabbableObject() = 0;

	virtual unsigned char getGrabPriority() = 0;
};
