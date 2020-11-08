#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Grabbable.generated.h"

UINTERFACE(MinimalAPI)
class UGrabbable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTCB_API IGrabbable
{
	GENERATED_BODY()

public:

	virtual bool isGrabbable() = 0;
	virtual void makeGrabbed() = 0;
	virtual void makeUngrabbed() = 0;

};
