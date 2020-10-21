#include "PlayerBasics.h"

const float PlayerBasics::playerWalkSpeed = 500.0f;

const float PlayerBasics::playerSize = 50.0f;

const float PlayerBasics::playerBaseGravity = 4.0f;
const float PlayerBasics::playerFastGravity = 1.5f * PlayerBasics::playerBaseGravity;

const float PlayerBasics::playerStartWorldLocationZ = 250.0f;
const float PlayerBasics::worldLocationProportionZ = 0.75f;

PlayerBasics::PlayerBasics()
{
	this->m_currentWorldLocationZ = PlayerBasics::playerStartWorldLocationZ;

	this->m_movementX = 0.0f;
	this->m_movementY = 0.0f;
	this->m_mobility = 1.0f;

	this->m_currentSize = PlayerBasics::playerSize;
	this->m_previousSize = PlayerBasics::playerSize;

	//this.m_controlRotation = 0;

	//FVector m_velocity; // characterMovement->Velocity = this->m_basics.m_velocity
	// TODO add other update variables

	this->m_jumpZVelocity = 0;
	this->m_airControl = 0;

	this->m_shouldJump = false;
}

float PlayerBasics::getAnimationPoint(float x)
{
	return x;// TODO adjust to sync with procedural animations (add additional parameters where needed)
}

void PlayerBasics::updateAttributes()
{
	this->m_previousSize = this->m_currentSize;
}
