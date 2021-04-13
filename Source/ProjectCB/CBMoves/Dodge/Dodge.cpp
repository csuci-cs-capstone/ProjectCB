#include "Dodge.h"

Dodge::Dodge(FPlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
}

void Dodge::onPress()
{
	if (this->m_playerBasics->getPlayerState() == FPlayerBasics::PLAYER_ALIVE)
	{
		if (!this->m_playerBasics->m_dodgeState)
			this->startDuck();

		this->m_buffer = true;
	}
}

void Dodge::onRelease()
{
	if (this->m_playerBasics->getPlayerState() == FPlayerBasics::PLAYER_ALIVE)
	{
		if (this->m_playerBasics->m_dodgeState != FPlayerBasics::DODGE_COOLDOWN && this->m_playerBasics->isGrounded())
			this->startDodge();

		this->m_buffer = false;
	}
}

void Dodge::update(float deltaTime)
{
	switch (this->m_playerBasics->m_dodgeState)
	{
		case FPlayerBasics::DODGE_STARTUP:
			this->startUpdate(deltaTime);
			break;
		case FPlayerBasics::DODGE_DUCK:
			this->duckUpdate(deltaTime);
			break;
		case FPlayerBasics::DODGE_IDLE:
			this->idleUpdate(deltaTime);
			break;
		case FPlayerBasics::DODGE_JUMP:
			this->jumpUpdate(deltaTime);
			break;
		case FPlayerBasics::DODGE_COOLDOWN:
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