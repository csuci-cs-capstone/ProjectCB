#include "Dodge_Release.h"
#include "Dodge.h"
#include "../../CBMath/Proportion.h"

// Dodge (Release)

const float Dodge_Release::DODGE_HEIGHT = 75.0f;
const float Dodge_Release::DODGE_MOBILITY = 0.0f; // TODO fix, allow for mobility mid air
const float Dodge_Release::DODGE_APEX_COLLIDER_SIZE = 25.0f;
const float Dodge_Release::DODGE_END_COLLIDER_SIZE = 50.0f;
const float Dodge_Release::DODGE_LAND_VELOCITY_RESET_PROPORTION = 0.25f;
const float Dodge_Release::DODGE_COOLDOWN_MOBILITY = 0.25f;

const short Dodge_Release::DODGE_FRAMES_TO_APEX = 7; // TODO change
const short Dodge_Release::DODGE_COOLDOWN_FRAMES = 30;

// Dive (Release + Direction)

const float Dodge_Release::DIVE_HEIGHT = Dodge_Release::DODGE_HEIGHT / 1.5f;
const float Dodge_Release::DIVE_MOBILITY = 0.0f;
const float Dodge_Release::DIVE_APEX_COLLIDER_SIZE = 25.0f;
const float Dodge_Release::DIVE_END_COLLIDER_SIZE = 50.0f;
const float Dodge_Release::DIVE_LAND_VELOCITY_RESET_PROPORTION = 0.25f;
const float Dodge_Release::DIVE_COOLDOWN_MOBILITY = 0.25f;

const short Dodge_Release::DIVE_FRAMES_TO_APEX = 5; // TODO change
const short Dodge_Release::DIVE_COOLDOWN_FRAMES = 30;

void Dodge::jumpUpdate(float deltaTime)
{
	this->m_playerBasics->m_currentMobility = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_MOBILITY,
		Dodge_Release::DIVE_MOBILITY);

	float prop = this->m_frame / this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_FRAMES_TO_APEX,
		Dodge_Release::DIVE_FRAMES_TO_APEX);

	if (prop >= 1)
	{
		if (prop >= 2)
		{
			this->m_playerBasics->m_currentHeight = this->m_dodgeProportion.getProportion(
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

			this->m_playerBasics->m_currentHeight = proportion.getProportion(endColliderSize, apexColliderSize);
		}
	}
	else
	{
		// Update Jump Rise

		prop = this->m_playerBasics->getAnimationPoint(prop);
		Proportion proportion(prop);

		float apexColliderSize = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_APEX_COLLIDER_SIZE,
			Dodge_Release::DIVE_APEX_COLLIDER_SIZE);

		this->m_playerBasics->m_currentHeight = proportion.getProportion(apexColliderSize,
			this->m_playerBasics->m_previousHeight);
	}

	if (this->m_playerBasics->isGrounded() && this->m_frame > 6)
	{
		// Start Cooldown

		this->m_playerBasics->m_dodgeState = FPlayerBasics::DODGE_COOLDOWN;
		this->m_frame = true;

		this->m_playerBasics->m_movement.resetMovement(this->m_dodgeProportion.getProportion(
			Dodge_Release::DODGE_LAND_VELOCITY_RESET_PROPORTION,
			Dodge_Release::DIVE_LAND_VELOCITY_RESET_PROPORTION));

		this->m_playerBasics->m_currentMobility = this->m_dodgeProportion.getProportion(
			Dodge_Release::DODGE_COOLDOWN_MOBILITY, Dodge_Release::DIVE_COOLDOWN_MOBILITY);

		this->m_playerBasics->updateAttributes();
	}
	else
		this->m_frame++;
}

void Dodge::cooldownUpdate(float deltaTime)
{
	short maxCooldownFrames = this->m_dodgeProportion.getProportion(Dodge_Release::DODGE_COOLDOWN_FRAMES,
		Dodge_Release::DIVE_COOLDOWN_FRAMES);

	if (this->m_frame >= maxCooldownFrames)
		this->endDodge();
	else
	{
		// Cooldown Update

		float prop = this->m_frame / (maxCooldownFrames * 1.0f);
		prop = this->m_playerBasics->getAnimationPoint(prop);
		Proportion proportion(prop);

		this->m_playerBasics->m_currentHeight = proportion.getProportion(FPlayerBasics::PLAYER_HEIGHT,
			this->m_playerBasics->m_previousHeight);

		this->m_frame++;
	}
}

void Dodge::endDodge()
{
	this->m_playerBasics->m_dodgeState = FPlayerBasics::DODGE_OFF;
	this->m_frame = false;

	this->m_playerBasics->m_currentMobility = 1.0f;
	this->m_playerBasics->m_currentHeight = FPlayerBasics::PLAYER_HEIGHT;

	this->m_playerBasics->updateAttributes();
}