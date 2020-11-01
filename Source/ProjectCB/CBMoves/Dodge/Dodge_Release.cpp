#include "Dodge_Release.h"
#include "Dodge.h"
#include "../../CBMath/Proportion.h"

// Dodge (Release)

const float Dodge_Release::DODGE_HEIGHT = 75.0f;
const float Dodge_Release::DODGE_MOBILITY = 0.5f; // TODO fix, allow for mobility mid air
const float Dodge_Release::DODGE_APEX_COLLIDER_SIZE = 25.0f;
const float Dodge_Release::DODGE_END_COLLIDER_SIZE = 50.0f;
const float Dodge_Release::DODGE_COOLDOWN_MOBILITY = 0.0f;

const short Dodge_Release::DODGE_COOLDOWN_FRAMES = 60;
const short Dodge_Release::DODGE_FRAMES_TO_APEX = 7; // TODO change

// Dive (Release + Direction)

const float Dodge_Release::DIVE_HEIGHT = Dodge_Release::DODGE_HEIGHT / 1.5f;
const float Dodge_Release::DIVE_HORIZONTAL_VELOCITY = 1.75f * Movement::playerWalkSpeed;
const float Dodge_Release::DIVE_MOBILITY = 0.5f;
const float Dodge_Release::DIVE_APEX_COLLIDER_SIZE = 25.0f;
const float Dodge_Release::DIVE_END_COLLIDER_SIZE = 50.0f;
const float Dodge_Release::DIVE_COOLDOWN_MOBILITY = 0.0f;

const short Dodge_Release::DIVE_COOLDOWN_FRAMES = 60;
const short Dodge_Release::DIVE_FRAMES_TO_APEX = 5; // TODO change

void Dodge::jumpUpdate(float deltaTime)
{
	this->m_playerBasics->m_currentMobility = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_MOBILITY,
		Dodge_Release::DIVE_MOBILITY);

	float prop = this->m_frame / this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_FRAMES_TO_APEX,
		Dodge_Release::DIVE_FRAMES_TO_APEX);

	// TODO make rotate for dive

	if (prop >= 1)
	{
		if (prop >= 2)
		{
			this->m_playerBasics->m_currentSize = this->m_dodgeProportion.getProportion(
				Dodge_Release::DODGE_END_COLLIDER_SIZE, Dodge_Release::DIVE_END_COLLIDER_SIZE);
		}
		else
		{
			// Update Jump Fall

			prop = this->m_playerBasics->getAnimationPoint(prop - 1);
			Proportion proportion(prop);

			float apexColliderSize = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_APEX_COLLIDER_SIZE,
				Dodge_Release::DIVE_APEX_COLLIDER_SIZE);

			float endColliderSize = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_END_COLLIDER_SIZE,
				Dodge_Release::DIVE_END_COLLIDER_SIZE);

			this->m_playerBasics->m_currentSize = proportion.getProportion(endColliderSize, apexColliderSize);
		}
	}
	else
	{
		// Update Jump Rise

		prop = this->m_playerBasics->getAnimationPoint(prop);
		Proportion proportion(prop);

		float apexColliderSize = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_APEX_COLLIDER_SIZE,
			Dodge_Release::DIVE_APEX_COLLIDER_SIZE);

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
	this->m_playerBasics->m_currentMobility = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_COOLDOWN_MOBILITY,
		Dodge_Release::DIVE_COOLDOWN_MOBILITY);

	short maxCooldownFrames = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_COOLDOWN_FRAMES,
		Dodge_Release::DIVE_COOLDOWN_FRAMES);

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