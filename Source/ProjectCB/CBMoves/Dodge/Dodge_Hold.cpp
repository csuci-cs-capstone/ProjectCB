#include "Dodge.h"
#include "Dodge_Hold.h"

const float Dodge_Hold::mobility = 0.5f;
const float Dodge_Hold::colliderSize = 25.0f;

const short Dodge_Hold::startupFrames = 6;
const short Dodge_Hold::actionFrames = 9;

void Dodge::startDuck()
{
	// Start Duck

	this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_STARTUP;
	this->m_frame = true;

	this->m_playerBasics->updateAttributes();
}

void Dodge::startDodge()
{
	const FRotator YawRotation(0, this->m_playerBasics->m_controlRotation.Yaw, 0);

	FVector direction = (this->m_playerBasics->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
		+ (this->m_playerBasics->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

	direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

	this->m_dodgeProportion.m_proportion = 1 - direction.Size();

	this->m_playerBasics->m_velocity = (this->m_dodgeProportion.invProp() * Dodge_Release::diveHorizontalVelocity)
		* direction;

	this->m_playerBasics->m_jumpZVelocity = this->m_dodgeProportion.getProportion(
		this->m_playerBasics->getJumpVelocity(Dodge_Release::dodgeHeight),
		this->m_playerBasics->getJumpVelocity(Dodge_Release::diveHeight));

	this->m_playerBasics->m_shouldJump = true;

	// Start Jump

	this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_JUMP;
	this->m_frame = true;

	this->m_playerBasics->updateAttributes();
}

void Dodge::startUpdate(float deltaTime)
{
	if (this->m_frame >= Dodge_Hold::startupFrames)
	{
		// Start Duck

		this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_DUCK;
		this->m_frame = true;

		this->m_playerBasics->updateAttributes();
	}
	else
		this->m_frame++;
}

void Dodge::duckUpdate(float deltaTime)
{
	if (this->m_frame >= Dodge_Hold::actionFrames)
	{
		this->m_playerBasics->m_currentMobility = Dodge_Hold::mobility;
		this->m_playerBasics->m_currentSize = Dodge_Hold::colliderSize;

		// Start Idle

		this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_IDLE;
		this->m_frame = true;

		this->m_playerBasics->updateAttributes();
	}
	else
	{
		float prop = this->m_frame / (Dodge_Hold::actionFrames * 1.0f);
		prop = this->m_playerBasics->getAnimationPoint(prop);
		Proportion proportion(prop);

		this->m_playerBasics->m_currentMobility = proportion.getProportion(Dodge_Hold::mobility,
			this->m_playerBasics->m_previousMobility);

		this->m_playerBasics->m_currentSize = proportion.getProportion(Dodge_Hold::colliderSize,
			this->m_playerBasics->m_previousSize);

		this->m_frame++;
	}
}

void Dodge::idleUpdate(float deltaTime) {}