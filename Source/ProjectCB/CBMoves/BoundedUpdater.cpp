#include "BoundedUpdater.h"

BoundedUpdater::BoundedUpdater(unsigned short totalFrames) : m_totalFrames(totalFrames) {}

void BoundedUpdater::run(float deltaTime)
{
	if (this->getFrame() >= this->m_totalFrames)
		this->end();
	
	else
		this->action(deltaTime, this->getFrame() / (this->m_totalFrames * 1.0f));
}
