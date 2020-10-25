#include "Dodge.h"
#include "Dodge_Release.h"

// Dodge (Release)

const float Dodge_Release::dodgeHeight = 1;
const float Dodge_Release::dodgeControl = 0.0f;
const float Dodge_Release::dodgeApexColliderSize = 25.0f;
const float Dodge_Release::dodgeEndColliderSize = 50.0f;
const float Dodge_Release::dodgeCooldownColliderSize = 50.f;

const short Dodge_Release::dodgeCooldownFrames = 60;
const short Dodge_Release::dodgeFramesToApex = 7;

// Dive (Release + Direction)

const float Dodge_Release::diveHeight = Dodge_Release::dodgeHeight / 2;
const float Dodge_Release::diveHorizontalVelocity = 1.75f * PlayerBasics::playerWalkSpeed;
const float Dodge_Release::diveControl = 0.0f;
const float Dodge_Release::diveApexColliderSize = 25.0f;
const float Dodge_Release::diveEndColliderSize = 50.0f;
const float Dodge_Release::diveCooldownColliderSize = 50.f;

const short Dodge_Release::diveCooldownFrames = 60;
const short Dodge_Release::diveFramesToApex = 5;

// Immutable

const float Dodge_Release::dodgeVelocity = sqrt(400000 * Dodge_Release::dodgeHeight * PlayerBasics::playerBaseGravity);
const float Dodge_Release::diveVerticalVelocity = sqrt(400000 * Dodge_Release::diveHeight * PlayerBasics::playerBaseGravity);

float Dodge::dodgeProportion(float dodgeValue, float diveValue)
{
	return (1 - this->m_diveProportion) * dodgeValue + this->m_diveProportion * diveValue;
}

void Dodge::dodgeActionUpdate()
{
	this->m_playerBasics->m_mobility = 1; // TODO make variable

	float proportion = this->m_frame / this->dodgeProportion(Dodge_Release::dodgeFramesToApex,
		Dodge_Release::diveFramesToApex);

	// TODO make rotate for dive

	if (proportion >= 1)
	{
		if (proportion >= 2)
		{
			this->m_playerBasics->m_currentSize = this->dodgeProportion(Dodge_Release::dodgeEndColliderSize,
				Dodge_Release::diveEndColliderSize);
		}
		else
		{
			this->m_state = FALL;
			proportion = this->m_playerBasics->getAnimationPoint(proportion - 1);

			float apexColliderSize = this->dodgeProportion(Dodge_Release::dodgeApexColliderSize,
				Dodge_Release::diveApexColliderSize);

			float endColliderSize = this->dodgeProportion(Dodge_Release::dodgeEndColliderSize,
				Dodge_Release::diveEndColliderSize);

			this->m_playerBasics->m_currentSize = ((1 - proportion) * apexColliderSize) + (proportion * endColliderSize);
		}
	}
	else
	{
		proportion = this->m_playerBasics->getAnimationPoint(proportion);

		float apexColliderSize = this->dodgeProportion(Dodge_Release::dodgeApexColliderSize,
			Dodge_Release::diveApexColliderSize);

		this->m_playerBasics->m_currentSize = (1 - proportion) * this->m_playerBasics->m_previousSize
			+ proportion * apexColliderSize;
	}

	if (this->m_playerBasics->m_grounded)
	{
		// Start Cooldown
		this->m_state = COOLDOWN;
		this->m_frame = true;

		this->m_playerBasics->updateAttributes();
	}
	else
		this->m_frame++;
}

void Dodge::dodgeCooldownUpdate()
{
	this->m_playerBasics->m_mobility = 0; // TODO make variable

	short maxCooldownFrames = this->dodgeProportion(Dodge_Release::dodgeCooldownFrames, Dodge_Release::diveCooldownFrames);

	if (this->m_frame >= maxCooldownFrames)
	{
		// End Dodge

		this->m_state = OFF;
		this->m_frame = false;

		this->m_playerBasics->m_currentSize = this->dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
			Dodge_Release::diveCooldownColliderSize);

		this->m_playerBasics->m_mobility = 1; // TODO make variable

		this->m_playerBasics->updateAttributes();
	}
	else
	{
		// Cooldown Update

		this->m_frame++;

		float proportion = this->m_playerBasics->getAnimationPoint(this->m_frame / (maxCooldownFrames * 1.0f));

		float colliderSize = this->dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
			Dodge_Release::diveCooldownColliderSize);

		this->m_playerBasics->m_currentSize = ((1 - proportion) * this->m_playerBasics->m_previousSize)
			+ (proportion * colliderSize);
	}
}