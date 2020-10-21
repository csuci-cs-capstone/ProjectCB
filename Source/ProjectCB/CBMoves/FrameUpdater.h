#pragma once

#include "CoreMinimal.h"

class PROJECTCB_API FrameUpdater
{
private:

	unsigned short m_frame;

protected:

	FrameUpdater();

	virtual ~FrameUpdater();

	virtual void run(float deltaTime) = 0;

	virtual void onStart();

	virtual void onEnd();

	unsigned short getFrame();

	bool shouldUpdate();

	void update(float deltaTime);

	void start();

	void end();

};
