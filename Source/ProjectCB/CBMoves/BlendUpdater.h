#pragma once

#include "CoreMinimal.h"
#include "FrameUpdater.h"

class PROJECTCB_API BlendUpdater : public FrameUpdater
{
private:

	const unsigned short m_totalFrames;

protected:

	BlendUpdater(unsigned short totalFrames);

	virtual void set(float deltaTime) = 0;

	virtual void blend(float deltaTime, float amount) = 0;

	void run(float deltaTime);
};
