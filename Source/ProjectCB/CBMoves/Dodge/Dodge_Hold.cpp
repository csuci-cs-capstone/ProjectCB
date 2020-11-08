#include "Dodge.h"

const float Dodge_Hold::MOBILITY = 0.5f;
const float Dodge_Hold::COLLIDER_HEIGHT = 25.0f;

const short Dodge_Hold::STARTUP_FRAMES = 6;
const short Dodge_Hold::ACTION_FRAMES = 9;

void Dodge::startDuck()
{
	// Start Duck

	this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_STARTUP;
	this->m_frame = true;

	this->m_playerBasics->updateAttributes();
}

void Dodge::startUpdate(float deltaTime) // Startup
{
	if (this->m_frame >= Dodge_Hold::STARTUP_FRAMES)
	{
		// Start Duck

		this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_DUCK;
		this->m_frame = true;

		this->m_playerBasics->updateAttributes();
	}
	else
		this->m_frame++;
}

void Dodge::duckUpdate(float deltaTime) // Hold
{
	if (this->m_playerBasics->getPlayerState() == PlayerBasics::PLAYER_ALIVE)
	{
		if (this->m_frame >= Dodge_Hold::ACTION_FRAMES)
		{
			this->m_playerBasics->m_currentMobility = Dodge_Hold::MOBILITY;
			this->m_playerBasics->m_currentHeight = Dodge_Hold::COLLIDER_HEIGHT;

			// Start Idle

			this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_IDLE;
			this->m_frame = true;

			this->m_playerBasics->updateAttributes();
		}
		else
		{
			float prop = this->m_frame / (Dodge_Hold::ACTION_FRAMES * 1.0f);
			prop = this->m_playerBasics->getAnimationPoint(prop);
			Proportion proportion(prop);

			this->m_playerBasics->m_currentMobility = proportion.getProportion(Dodge_Hold::MOBILITY,
				this->m_playerBasics->m_previousMobility);

			this->m_playerBasics->m_currentHeight = proportion.getProportion(Dodge_Hold::COLLIDER_HEIGHT,
				this->m_playerBasics->m_previousHeight);

			this->m_frame++;
		}
	}
	else
		this->endDodge();
}

void Dodge::idleUpdate(float deltaTime) {}

void Dodge::startDodge() // Release
{
	FVector direction = this->m_playerBasics->getInputDirection();
	direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;
	this->m_dodgeProportion.m_proportion = 1 - direction.Size();

	//this->m_playerBasics->m_velocity = (this->m_dodgeProportion.invProp() * Dodge_Release::DIVE_HORIZONTAL_VELOCITY)
	//	* direction;
	// TODO make sure velocity is appropriate

	this->m_playerBasics->m_currentMobility = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_MOBILITY,
		Dodge_Release::DIVE_MOBILITY);

	this->m_playerBasics->m_jumpZVelocity = this->m_dodgeProportion.getProportion(
		this->m_playerBasics->getJumpVelocity(Dodge_Release::DODGE_HEIGHT),
		this->m_playerBasics->getJumpVelocity(Dodge_Release::DIVE_HEIGHT));

	this->m_playerBasics->m_shouldJump = true;

	// Start Jump

	this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_JUMP;
	this->m_frame = true;

	this->m_playerBasics->updateAttributes();
}