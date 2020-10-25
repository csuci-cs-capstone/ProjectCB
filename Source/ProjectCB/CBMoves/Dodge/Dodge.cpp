#include "Dodge.h"

Dodge::Dodge(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
}

void Dodge::onPress()
{
	if (!this->m_state)
		this->startDuck();

	this->m_buffer = true;
}

void Dodge::onRelease()
{
	if (this->m_state != COOLDOWN && this->m_playerBasics->m_grounded)
		this->startDodge();

	this->m_buffer = false;
}

void Dodge::update(float deltaTime)
{
	switch (this->m_state)
	{
		case START:
			this->startUpdate(deltaTime);
			break;
		case DUCK:
			this->duckUpdate(deltaTime);
			break;
		case IDLE:
			this->idleUpdate(deltaTime);
			break;
		case JUMP:
			this->jumpUpdate(deltaTime);
			break;
		case COOLDOWN:
			this->cooldownUpdate(deltaTime);
			break;
		default:
			if (this->m_buffer)
			{
				this->startDuck();
				this->m_buffer = false;
			}
			break;
	}
}