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

	virtual void makeGrabbed() = 0; // TODO combine? (would be more efficient, but less reliable)
	virtual void setGrabbedPosition(FVector position) = 0;
	// could theoretically be done reliably, but would require additional work

	virtual void launchRelease(FVector direction) = 0;
};
