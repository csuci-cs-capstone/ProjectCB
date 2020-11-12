#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrabbableObject.generated.h"

UINTERFACE(MinimalAPI)
class UGrabbableObject : public UInterface
{
	GENERATED_BODY()
};

class PROJECTCB_API IGrabbableObject
{
	GENERATED_BODY()

public:

	virtual bool isGrabbable() = 0;

	virtual void makeGrabbed() = 0;

	virtual void launchRelease(FVector direction) = 0;

	virtual void setGrabbed(FVector position, FRotator rotation) = 0;

};
