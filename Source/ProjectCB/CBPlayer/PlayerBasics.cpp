#include "PlayerBasics.h"

const float PlayerBasics::playerSize = 50.0f;

const float PlayerBasics::playerBaseGravity = 4.0f;
const float PlayerBasics::playerFastGravity = 1.5f * PlayerBasics::playerBaseGravity;

const float PlayerBasics::playerStartWorldLocationZ = 250.0f;
const float PlayerBasics::worldLocationProportionZ = 0.75f;

PlayerBasics::PlayerBasics()
{
	this->m_playerState = PLAYER_ALIVE;
	this->m_dodgeState = DODGE_OFF;

	this->m_currentWorldLocationZ = PlayerBasics::playerStartWorldLocationZ;

	this->m_movementX = 0.0f;
	this->m_movementY = 0.0f;

	this->m_currentMobility = 1.0f;
	this->m_previousMobility = this->m_currentMobility;

	this->m_currentSize = PlayerBasics::playerSize;
	this->m_previousSize = this->m_currentSize;

	this->m_jumpZVelocity = 0;

	this->m_shouldJump = false;
}

float PlayerBasics::getJumpVelocity(float height)
{
	return sqrt(5000 * (height + ((PlayerBasics::playerSize - this->m_currentSize) / 20.0f))
		* PlayerBasics::playerBaseGravity);
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
