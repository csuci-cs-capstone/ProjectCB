#include "FrameUpdater.h"

FrameUpdater::FrameUpdater() {}

FrameUpdater::~FrameUpdater() {}

void FrameUpdater::onStart() {}

void FrameUpdater::onEnd() {}

unsigned short FrameUpdater::getFrame()
{
	return this->m_frame;
}

bool FrameUpdater::shouldUpdate()
{
	return this->m_frame > 0;
}

void FrameUpdater::start()
{
	this->m_frame = true;

	onStart();
}

void FrameUpdater::end()
{
	this->m_frame = 0;

	//if (GEngine)
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("STOPPING")));

	onEnd();
}

void FrameUpdater::update(float deltaTime)
{
	if (this->shouldUpdate())
	{
		run(deltaTime);

		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Update %d"),
		//		this->getFrame()));

		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("UPDATING")));

		this->m_frame++;
	}
}



