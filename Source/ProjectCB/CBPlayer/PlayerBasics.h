#pragma once

const float g_playerWalkSpeed = 500.0f;
const float g_playerBaseGravity = 4.0f;
const float g_playerFastGravity = g_playerBaseGravity * 1.5f;
const float g_playerStartWorldLocationZ = 250.0f;

//float getAnimationPoint(float x);

#include "CoreMinimal.h"

struct PROJECTCB_API PlayerBasics
{
public:

	// TODO 'fix' the camera at the original center of character (based on character size) [smoothly update]

	float m_currentWorldLocationZ = g_playerStartWorldLocationZ;
	float m_worldLocationProportionZ = 0.75f;

	float m_movementX;
	float m_movementY;
	float m_mobility;

	float m_currentSize = 50.0f;
	float m_previousSize;

	float getAnimationPoint(float x);
};
