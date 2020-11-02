#include "PlayerBasics.h"

const FVector2D PlayerBasics::MAX_MAP_POSITION(2150.0f, 700.0f);
const float PlayerBasics::MIN_MAP_POSITION_Z = -200.0f;

const float PlayerBasics::PLAYER_SPAWN_POSITION_Z = 100.0f;

const float PlayerBasics::PLAYER_SIZE = 50.0f;

const float PlayerBasics::PLAYER_BASE_GRAVITY = 4.0f;
const float PlayerBasics::PLAYER_FAST_GRAVITY = 1.5f * PlayerBasics::PLAYER_BASE_GRAVITY;

const float PlayerBasics::PLAYER_START_WORLD_LOCATION_Z = 250.0f;
const float PlayerBasics::WORLD_LOCATION_PROPORTION_Z = 0.75f;

PlayerBasics::PlayerBasics()
{
	this->m_playerState = PLAYER_ALIVE;
	this->m_dodgeState = DODGE_OFF;

	this->m_currentWorldLocationZ = PlayerBasics::PLAYER_START_WORLD_LOCATION_Z;

	this->m_movementX = 0.0f;
	this->m_movementY = 0.0f;

	//this->m_movement.setPlayerBasics(this);

	this->m_currentMobility = 1.0f;
	this->m_previousMobility = this->m_currentMobility;

	this->m_currentSize = PlayerBasics::PLAYER_SIZE;
	this->m_previousSize = this->m_currentSize;

	this->m_jumpZVelocity = 0;

	this->m_shouldJump = false;
}

float PlayerBasics::getJumpVelocity(float height)
{
	return sqrt(5000 * (height + ((PlayerBasics::PLAYER_SIZE - this->m_currentSize) / 20.0f))
		* PlayerBasics::PLAYER_BASE_GRAVITY);
}

float PlayerBasics::getAnimationPoint(float x)
{
	return x;// TODO adjust to sync with procedural animations (add additional parameters where needed)
}

void PlayerBasics::updateAttributes()
{
	this->m_previousMobility = this->m_currentMobility;
	this->m_previousSize = this->m_currentSize;
}

void PlayerBasics::updateGroundState(bool grounded)
{
	this->m_grounded = grounded;

	this->m_movement.isGrounded(this->m_grounded);
}

bool PlayerBasics::isGrounded()
{
	return this->m_grounded;
}
