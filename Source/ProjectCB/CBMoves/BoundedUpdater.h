#pragma once

#include "CoreMinimal.h"
#include "FrameUpdater.h"

class PROJECTCB_API BoundedUpdater : public FrameUpdater
{
private:

	const unsigned short m_totalFrames;

protected:

	BoundedUpdater(unsigned short totalFrames);

	virtual void action(float deltaTime, float percentComplete) = 0;

	void run(float deltaTime);
};
