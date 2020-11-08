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
}

bool Throw::isGrabbable(AActor* actor)
{
	if (actor)
	{
		if (actor->IsA(ACB_DodgeballProjectile::StaticClass()))
		{
			ACB_DodgeballProjectile* dodgeball = (ACB_DodgeballProjectile*)actor;

			if (dodgeball->getBallState() == ACB_DodgeballProjectile::BALL_PROJECTILE)
				return true;
		}
		else if (actor->IsA(ACB_PlayerCharacter::StaticClass()))
		{
			ACB_PlayerCharacter* player = (ACB_PlayerCharacter*)actor;

			if (player->m_basics.getPlayerState() == PlayerBasics::PLAYER_ALIVE)
				return true;
		}
	}
	
	return false;
}