#include "Dodge.h"
#include "Dodge_Hold.h"

const float Dodge_Hold::mobility = 0.0f;
const float Dodge_Hold::colliderSize = 25.0f;

const short Dodge_Hold::startupFrames = 6;
const short Dodge_Hold::actionFrames = 24;

void Dodge::startDuck()
{
	// Start Duck

	this->m_state = START;
	this->m_frame = true;

	this->m_playerBasics->updateAttributes();
}

void Dodge::startDodge()
{
	const FRotator YawRotation(0, this->m_playerBasics->m_controlRotation.Yaw, 0);

	FVector direction = (this->m_playerBasics->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
		+ (this->m_playerBasics->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

	direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

	this->m_diveProportion = direction.Size();

	this->m_playerBasics->m_velocity = (this->m_diveProportion * Dodge_Release::diveHorizontalVelocity)
		* direction;

	this->m_playerBasics->m_jumpZVelocity = this->dodgeProportion(
		this->m_playerBasics->getJumpVelocity(Dodge_Release::dodgeHeight),
		this->m_playerBasics->getJumpVelocity(Dodge_Release::diveHeight));

	this->m_playerBasics->m_shouldJump = true;

	// Start Jump

	this->m_state = JUMP;
	this->m_frame = true;

	this->m_playerBasics->updateAttributes();
}

void Dodge::startUpdate(float deltaTime)
{
	if (this->m_frame >= Dodge_Hold::startupFrames)
	{
		// Start Duck

		this->m_state = DUCK;
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

		this->m_state = IDLE;
		this->m_frame = true;

		this->m_playerBasics->updateAttributes();
	}
	else
	{
		float proportion = this->m_frame / (Dodge_Hold::actionFrames * 1.0f);

		proportion = this->m_playerBasics->getAnimationPoint(proportion);

		this->m_playerBasics->m_currentMobility = (1 - proportion) * this->m_playerBasics->m_previousMobility
			+ proportion * Dodge_Hold::mobility;

		this->m_playerBasics->m_currentSize = (1 - proportion) * this->m_playerBasics->m_previousSize
			+ proportion * Dodge_Hold::colliderSize;

		this->m_frame++;
	}
}

void Dodge::idleUpdate(float deltaTime) {}