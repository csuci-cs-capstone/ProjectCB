#include "Dodge.h"

Dodge::Dodge(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
}

void Dodge::onPress()
{
	if (!this->m_playerBasics->m_dodgeState)
		this->startDuck();

	this->m_buffer = true;
}

void Dodge::onRelease()
{
	if (this->m_playerBasics->m_dodgeState != PlayerBasics::DodgeState::COOLDOWN && this->m_playerBasics->m_grounded)
		this->startDodge();

	this->m_buffer = false;
}

void Dodge::update(float deltaTime)
{
	switch (this->m_playerBasics->m_dodgeState)
	{
		case PlayerBasics::DodgeState::START:
			this->startUpdate(deltaTime);
			break;
		case PlayerBasics::DodgeState::DUCK:
			this->duckUpdate(deltaTime);
			break;
		case PlayerBasics::DodgeState::IDLE:
			this->idleUpdate(deltaTime);
			break;
		case PlayerBasics::DodgeState::JUMP:
			this->jumpUpdate(deltaTime);
			break;
		case PlayerBasics::DodgeState::COOLDOWN:
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