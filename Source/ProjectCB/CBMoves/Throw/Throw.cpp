#include "Throw.h"

const FVector Throw::THROW_DIRECTION = FVector(1, 0, 0.025).GetUnsafeNormal();

Throw::Throw(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
	this->m_grabbableList = new GrabbableList();
}

void Throw::onPress()
{
	if (!this->m_playerBasics->m_throwState)
	{
		if (this->m_grabbedObject)
			this->m_playerBasics->m_throwState = PlayerBasics::THROW_STARTUP;

		else
			this->m_playerBasics->m_throwState = PlayerBasics::CATCH_STARTUP;
	}
}

void Throw::onRelease()
{
	if (this->m_playerBasics->m_throwState == PlayerBasics::THROW_AIM) // TODO should buffer for startup
	{
		// THROW BALL

		this->m_grabbedObject->launchRelease( // TODO should probably ensure it exists first
			this->m_playerBasics->m_movement.getPlayerRotation().RotateVector(FVector(1.0f,0.0f,0.0f)));
		
			// TODO launch ball in current player direction
		this->m_grabbedObject = nullptr;
		// TODO might want to ensure that there is no way grabbable can not be null when in throw state

		this->m_playerBasics->m_throwState = PlayerBasics::THROW_COOLDOWN;
	}

	else if (this->m_playerBasics->m_throwState == PlayerBasics::CATCH_AIM) // TODO should buffer for startup
		this->m_playerBasics->m_throwState = PlayerBasics::CATCH_COOLDOWN;
}

void Throw::update(FVector playerPosition, FRotator playerRotation, float deltaTime)
{
	switch (this->m_playerBasics->m_throwState)
	{
	case PlayerBasics::CATCH_STARTUP:
		this->catchStartUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_STARTUP:
		this->throwStartUpdate(deltaTime);
		break;
	case PlayerBasics::CATCH_AIM:
		this->catchAimUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_AIM:
		this->throwAimUpdate(deltaTime);
		break;
	case PlayerBasics::CATCH_COOLDOWN:
		this->catchCooldownUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_COOLDOWN:
		this->throwCooldownUpdate(deltaTime);
		break;
	default:
		//if (this->m_buffer) // TODO create buffer?
		//{
		//	this->startDuck();
		//	this->m_buffer = false;
		//}
		break;
	}

	if (this->m_grabbedObject)
		this->m_grabbedObject->setGrabbedPosition(playerPosition + playerRotation.RotateVector(FVector(75.0f, 0.0f, 0.0f)));
}
