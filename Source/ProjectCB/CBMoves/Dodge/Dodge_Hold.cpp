#include "Dodge_Hold.h"

const float Dodge_Hold::colliderSize = 25.0f;

const float Dodge_Hold::startupMobility = 0.0f;
const float Dodge_Hold::actionMobility = 0.0f;

const short Dodge_Hold::startupFrames = 6;
const short Dodge_Hold::actionFrames = 24;

Dodge_Hold::Dodge_Hold()
{
	this->m_frame = false;
}

Dodge_Hold::ActionUpdater::ActionUpdater(Dodge_Hold* const dodgeHold, unsigned short totalFrames)
	: BoundedUpdater(totalFrames), m_dodgeHold(dodgeHold) {}

void Dodge_Hold::ActionUpdater::onEnd()
{
	this->m_dodgeHold->m_playerBasics->m_mobility = Dodge_Hold::actionMobility;

	this->m_dodgeHold->m_playerBasics->m_currentSize = Dodge_Hold::colliderSize;
}

void Dodge_Hold::ActionUpdater::action(float deltaTime, float amount)
{
	amount = this->m_dodgeHold->m_playerBasics->getAnimationPoint(amount);

	this->m_dodgeHold->m_playerBasics->m_currentSize = (1 - amount) * this->m_dodgeHold->m_playerBasics->m_previousSize
		+ amount * Dodge_Hold::colliderSize;
}

void Dodge_Hold::update() // TODO make in terms of BufferedUpdater, BlendUpdater, UnboundedUpdater
{
	if (this->m_frame == 1)
		this->m_playerBasics->m_previousSize = this->m_playerBasics->m_currentSize;

	if (this->m_frame >= Dodge_Hold::startupFrames)
	{
		if (this->m_frame >= (Dodge_Hold::startupFrames + Dodge_Hold::actionFrames))
		{
			this->m_playerBasics->m_mobility = Dodge_Hold::actionMobility;

			this->m_playerBasics->m_currentSize = Dodge_Hold::colliderSize;
		}
		else
		{
			float proportion = (this->m_frame - Dodge_Hold::startupFrames) / (Dodge_Hold::actionFrames * 1.0f);

			proportion = this->m_playerBasics->getAnimationPoint(proportion);

			this->m_playerBasics->m_currentSize = (1 - proportion) * this->m_playerBasics->m_previousSize
				+ proportion * Dodge_Hold::colliderSize;
		}
	}

	this->m_frame++;
}