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
	this->m_frame = false;

	onEnd();
}

void FrameUpdater::update(float deltaTime)
{
	run(deltaTime);

	this->m_frame++;
}



