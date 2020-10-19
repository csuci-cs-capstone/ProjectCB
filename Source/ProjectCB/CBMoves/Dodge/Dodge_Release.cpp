#include "Dodge_Release.h"

// Dodge (Release)

const float Dodge_Release::dodgeHeight = 1;
const float Dodge_Release::dodgeControl = 0.0f;
const float Dodge_Release::dodgeApexColliderSize = 25.0f;
const float Dodge_Release::dodgeEndColliderSize = 50.0f;
const float Dodge_Release::dodgeCooldownColliderSize = 50.f;

const short Dodge_Release::dodgeCooldownFrames = 15;
const short Dodge_Release::dodgeFramesToApex = 7;

// Dive (Release + Direction)

const float Dodge_Release::diveHeight = Dodge_Release::dodgeHeight / 2;
const float Dodge_Release::diveHorizontalVelocity = 1.75f * g_playerWalkSpeed;
const float Dodge_Release::diveControl = 0.0f;
const float Dodge_Release::diveApexColliderSize = 25.0f;
const float Dodge_Release::diveEndColliderSize = 50.0f;
const float Dodge_Release::diveCooldownColliderSize = 50.f;

const short Dodge_Release::diveCooldownFrames = 15;
const short Dodge_Release::diveFramesToApex = 5;

// Immutable

const float Dodge_Release::dodgeVelocity = sqrt(400000 * Dodge_Release::dodgeHeight * g_playerBaseGravity);
const float Dodge_Release::diveVerticalVelocity = sqrt(400000 * Dodge_Release::diveHeight * g_playerBaseGravity);

float Dodge_Release::dodgeProportion(float dodgeValue, float diveValue)
{
	return (1 - this->m_diveProportion) * dodgeValue + this->m_diveProportion * diveValue;
}

void Dodge_Release::dodgeCooldownUpdate(PlayerBasics playerBasics)
{
	playerBasics.m_mobility = 0; // TODO make variable

	short maxCooldownFrames = this->dodgeProportion(Dodge_Release::dodgeCooldownFrames, Dodge_Release::diveCooldownFrames);

	if (this->m_dodgeCooldownFrame >= maxCooldownFrames)
	{
		this->m_dodgeCooldownFrame = false;

		playerBasics.m_currentSize = this->dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
			Dodge_Release::diveCooldownColliderSize);

		playerBasics.m_mobility = 1; // TODO make variable
	}
	else
	{
		this->m_dodgeCooldownFrame++;

		float proportion = playerBasics.getAnimationPoint(this->m_dodgeCooldownFrame / (maxCooldownFrames * 1.0f));

		float colliderSize = this->dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
			Dodge_Release::diveCooldownColliderSize);

		playerBasics.m_currentSize = ((1 - proportion) * playerBasics.m_previousSize) + (proportion * colliderSize);
	}
}