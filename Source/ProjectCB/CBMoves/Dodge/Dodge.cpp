#include "Dodge.h"

Dodge::Dodge(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
}

void Dodge::onPress()
{
	if (!this->m_state)
	{
		this->m_state = START;
		this->m_frame = true;

		this->m_playerBasics->updateAttributes();
	}
}

void Dodge::onRelease()
{
	switch (this->m_state)
	{
		case START:
			this->startDodge();
			break;
		case DUCK:
			this->startDodge();
			break;
		case IDLE:
			this->startDodge();
			break;
		case JUMP:
			this->startDodge();
			break;
		case FALL:
			this->startDodge();
			break;
		default:
			break;
	}
}

void Dodge::update(float deltaTime)
{
	switch (this->m_state)
	{
		case START:
			this->holdUpdate(deltaTime);
			break;
		case DUCK:
			this->holdUpdate(deltaTime);
			break;
		case IDLE:
			this->holdUpdate(deltaTime);
			break;
		case JUMP:
			this->dodgeActionUpdate();
			break;
		case FALL:
			this->dodgeActionUpdate();
			break;
		case COOLDOWN:
			this->dodgeCooldownUpdate();
			break;
		default:
			break;
	}
}