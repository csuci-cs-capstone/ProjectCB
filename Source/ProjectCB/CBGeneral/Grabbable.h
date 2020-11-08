#pragma once

#include "CoreMinimal.h"

class PROJECTCB_API Grabbable
{
public:

	virtual void makeUngrabbed() = 0;

	virtual void makeGrabbed() = 0;

};
