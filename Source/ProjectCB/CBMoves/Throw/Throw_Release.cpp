#include "Throw_Release.h"
#include "Throw.h"

void Throw::catchCooldownUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = PlayerBasics::THROW_OFF; // TODO implement cooldown
}

void Throw::throwCooldownUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = PlayerBasics::THROW_OFF; // TODO implement cooldown
}