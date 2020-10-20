#include "PlayerBasics.h"

const float PlayerBasics::playerWalkSpeed = 500.0f;
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

	this->m_currentSize = 50.0f; // TODO make startSize
	this->m_previousSize;
}

float PlayerBasics::getAnimationPoint(float x) // ranges between 0 and 1
{
	return x;// TODO adjust to sync with procedural animations (add additional parameters where needed)
}
