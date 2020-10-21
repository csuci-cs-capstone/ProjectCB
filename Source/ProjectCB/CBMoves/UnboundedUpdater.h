#pragma once

#include "CoreMinimal.h"
#include "FrameUpdater.h"

class PROJECTCB_API UnboundedUpdater : public FrameUpdater
{
protected:

	void run(float deltaTime);
};
