#include "Throw.h"
#include "../../CBPlayer/CB_PlayerCharacter.h"
#include "../../CBObjects/CB_DodgeballProjectile.h"

const FVector Throw::THROW_DIRECTION = FVector(1, 0, 0.025).GetUnsafeNormal();

Throw::Throw(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
}

void Throw::onPress()
{
	// TODO start grab/aim

	//if (actor->IsA(ACB_DodgeballProjectile::StaticClass()))
	//{
	//	// pickup dodgeball
	//}
	//else if (actor->IsA(ACB_PlayerCharacter::StaticClass()))
	//{
	//	// pickup player
	//}

	if (this->m_grabbedObject)
		this->m_playerBasics->m_throwState = PlayerBasics::THROW_STARTUP; // THROW
	else
		this->m_playerBasics->m_throwState = PlayerBasics::CATCH_STARTUP; // GRAB
}

void Throw::onRelease()
{
	if (this->m_playerBasics->m_throwState == PlayerBasics::THROW_AIM)
		this->m_playerBasics->m_throwState = PlayerBasics::THROW_COOLDOWN;

	else if (this->m_playerBasics->m_throwState == PlayerBasics::CATCH_AIM)
		this->m_playerBasics->m_throwState = PlayerBasics::CATCH_COOLDOWN;
}

void Throw::update(float deltaTime)
{
	switch (this->m_playerBasics->m_throwState)
	{
	case PlayerBasics::CATCH_STARTUP:
		//this->startUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_STARTUP:
		//this->duckUpdate(deltaTime);
		break;
	case PlayerBasics::CATCH_AIM:
		//this->idleUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_AIM:
		//this->jumpUpdate(deltaTime);
		break;
	case PlayerBasics::CATCH_COOLDOWN:
		//this->cooldownUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_COOLDOWN:
		break;
	default:
		//if (this->m_buffer)
		//{
		//	this->startDuck();
		//	this->m_buffer = false;
		//}
		break;
	}
}