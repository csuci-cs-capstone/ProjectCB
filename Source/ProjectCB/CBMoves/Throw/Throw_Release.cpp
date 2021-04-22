#include "Throw_Release.h"
#include "Throw.h"

void UThrow::catchCooldownUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = FPlayerBasics::THROW_OFF; // TODO implement cooldown
}

void UThrow::throwCooldownUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = FPlayerBasics::THROW_OFF; // TODO implement cooldown
}