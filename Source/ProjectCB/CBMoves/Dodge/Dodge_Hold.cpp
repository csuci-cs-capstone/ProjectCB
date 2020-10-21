
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

bool Dodge_Hold::isRunning()
{
	return this->m_startUpdater.shouldUpdate() || this->m_actionUpdater.shouldUpdate();
}

void Dodge_Hold::start()
{
	this->m_startUpdater.start();
}

void Dodge_Hold::end()
{
	if (this->m_startUpdater.shouldUpdate())
		this->m_startUpdater.end();

	else if (this->m_actionUpdater.shouldUpdate())
		this->m_actionUpdater.end();
}

void Dodge_Hold::update(float deltaTime)
{
	//if (this->m_startUpdater.shouldUpdate())
	//	this->m_startUpdater.update(deltaTime);

	//else if (this->m_actionUpdater.shouldUpdate())
	//	this->m_actionUpdater.update(deltaTime);

	if (this->m_frame == 1)
		this->m_playerBasics->updateAttributes();

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

// Start Updater

Dodge_Hold::StartUpdater::StartUpdater(Dodge_Hold* const dodgeHold, unsigned short totalFrames)
	: BoundedUpdater(totalFrames), m_dodgeHold(dodgeHold) {}

void Dodge_Hold::StartUpdater::onStart()
{
	this->m_dodgeHold->m_playerBasics->updateAttributes();
}

void Dodge_Hold::StartUpdater::onEnd()
{
	this->m_dodgeHold->m_actionUpdater.start();
}

void Dodge_Hold::StartUpdater::action(float deltaTime, float amount) {}

// Action Updater

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