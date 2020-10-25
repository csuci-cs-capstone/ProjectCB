#include "Dodge.h"
#include "Dodge_Hold.h"

const float Dodge_Hold::colliderSize = 25.0f;

const float Dodge_Hold::startupMobility = 0.0f;
const float Dodge_Hold::actionMobility = 0.0f;

const short Dodge_Hold::startupFrames = 6;
const short Dodge_Hold::actionFrames = 24;

void Dodge::holdUpdate(float deltaTime)
{
	if (this->m_frame >= Dodge_Hold::startupFrames)
	{
		if (this->m_frame >= (Dodge_Hold::startupFrames + Dodge_Hold::actionFrames))
		{
			this->m_playerBasics->m_mobility = Dodge_Hold::actionMobility;

			this->m_playerBasics->m_currentSize = Dodge_Hold::colliderSize;
		}
		else
		{
			float proportion = (this->m_frame - Dodge_Hold::startupFrames) / (Dodge_Hold::actionFrames * 1.0f);

			proportion = this->m_playerBasics->getAnimationPoint(proportion);

			this->m_playerBasics->m_currentSize = (1 - proportion) * this->m_playerBasics->m_previousSize
				+ proportion * Dodge_Hold::colliderSize;
		}
	}

	this->m_frame++;
}

void Dodge::startDodge()
{
	if (this->m_playerBasics->m_grounded)
	{
		this->m_state = JUMP;
		this->m_frame = true;

		const FRotator YawRotation(0, this->m_playerBasics->m_controlRotation.Yaw, 0);

		FVector direction = (this->m_playerBasics->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
			+ (this->m_playerBasics->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

		this->m_diveProportion = direction.Size();

		this->m_playerBasics->m_velocity = (this->m_diveProportion * Dodge_Release::diveHorizontalVelocity)
			* direction;

		this->m_playerBasics->m_jumpZVelocity = this->dodgeProportion(Dodge_Release::dodgeVelocity,
			Dodge_Release::diveVerticalVelocity);

		this->m_playerBasics->m_airControl = this->dodgeProportion(Dodge_Release::dodgeControl,
			Dodge_Release::diveControl);

		this->m_playerBasics->m_shouldJump = true;

		this->m_playerBasics->updateAttributes();
	}
}