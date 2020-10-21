#include "BlendUpdater.h"

void BlendUpdater::run(float deltaTime)
{
	if (this->getFrame() >= this->m_totalFrames)
		this->set(deltaTime);

	else
	{
		float amount = this->getFrame() / (this->m_totalFrames * 1.0f);

		this->blend(deltaTime, amount);
	}
}