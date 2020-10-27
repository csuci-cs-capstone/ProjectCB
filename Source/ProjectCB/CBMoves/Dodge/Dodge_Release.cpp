#include "Dodge.h"
#include "Dodge_Release.h"
#include "../../CBMath/Proportion.h"

// Dodge (Release)

const float Dodge_Release::dodgeHeight = 75.0f;
const float Dodge_Release::dodgeMobility = 0.0f;
const float Dodge_Release::dodgeApexColliderSize = 25.0f; // 25
const float Dodge_Release::dodgeEndColliderSize = 50.0f;
const float Dodge_Release::dodgeCooldownMobility = 0.0f;

const short Dodge_Release::dodgeCooldownFrames = 60;
const short Dodge_Release::dodgeFramesToApex = 7; // TODO change

// Dive (Release + Direction)

const float Dodge_Release::diveHeight = Dodge_Release::dodgeHeight / 1.5f;
const float Dodge_Release::diveHorizontalVelocity = 1.75f * PlayerBasics::playerWalkSpeed;
const float Dodge_Release::diveMobility = 0.0f;
const float Dodge_Release::diveApexColliderSize = 25.0f;
const float Dodge_Release::diveEndColliderSize = 50.0f;
const float Dodge_Release::diveCooldownMobility = 0.0f;

const short Dodge_Release::diveCooldownFrames = 60;
const short Dodge_Release::diveFramesToApex = 5;

void Dodge::jumpUpdate(float deltaTime)
{
	this->m_playerBasics->m_currentMobility = this->m_dodgeProportion.getProportion(Dodge_Release::dodgeMobility,
		Dodge_Release::diveMobility);

	float prop = this->m_frame / this->m_dodgeProportion.getProportion(Dodge_Release::dodgeFramesToApex,
		Dodge_Release::diveFramesToApex);

	// TODO make rotate for dive

	if (prop >= 1)
	{
		if (prop >= 2)
		{
			this->m_playerBasics->m_currentSize = this->m_dodgeProportion.getProportion(Dodge_Release::dodgeEndColliderSize,
				Dodge_Release::diveEndColliderSize);
		}
		else
		{
			// Update Jump Fall

			prop = this->m_playerBasics->getAnimationPoint(prop - 1);
			Proportion proportion(prop);

			float apexColliderSize = this->m_dodgeProportion.getProportion(Dodge_Release::dodgeApexColliderSize,
				Dodge_Release::diveApexColliderSize);

			float endColliderSize = this->m_dodgeProportion.getProportion(Dodge_Release::dodgeEndColliderSize,
				Dodge_Release::diveEndColliderSize);

			this->m_playerBasics->m_currentSize = proportion.getProportion(endColliderSize, apexColliderSize);
		}
	}
	else
	{
		// Update Jump Rise

		prop = this->m_playerBasics->getAnimationPoint(prop);
		Proportion proportion(prop);

		float apexColliderSize = this->m_dodgeProportion.getProportion(Dodge_Release::dodgeApexColliderSize,
			Dodge_Release::diveApexColliderSize);

		this->m_playerBasics->m_currentSize = proportion.getProportion(apexColliderSize,
			this->m_playerBasics->m_previousSize);
	}

	if (this->m_playerBasics->m_grounded)
	{
		// Start Cooldown

		this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_COOLDOWN;
		this->m_frame = true;

		this->m_playerBasics->updateAttributes();
	}
	else
		this->m_frame++;
}

void Dodge::cooldownUpdate(float deltaTime)
{
	this->m_playerBasics->m_currentMobility = this->m_dodgeProportion.getProportion(Dodge_Release::dodgeCooldownMobility,
		Dodge_Release::diveCooldownMobility);

	short maxCooldownFrames = this->m_dodgeProportion.getProportion(Dodge_Release::dodgeCooldownFrames,
		Dodge_Release::diveCooldownFrames);

	if (this->m_frame >= maxCooldownFrames)
	{
		// End Dodge

		this->m_playerBasics->m_dodgeState = PlayerBasics::DODGE_OFF;
		this->m_frame = false;

		this->m_playerBasics->m_currentMobility = 1;
		this->m_playerBasics->m_currentSize = PlayerBasics::playerSize;

		this->m_playerBasics->updateAttributes();
	}
	else
	{
		// Cooldown Update

		float prop = this->m_frame / (maxCooldownFrames * 1.0f);
		prop = this->m_playerBasics->getAnimationPoint(prop);
		Proportion proportion(prop);

		this->m_playerBasics->m_currentSize = proportion.getProportion(PlayerBasics::playerSize,
			this->m_playerBasics->m_previousSize);

		this->m_frame++;
	}
}