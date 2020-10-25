#pragma once

#include "CoreMinimal.h"
#include <math.h>

struct PROJECTCB_API PlayerBasics
{
	// Mutable

	static const float playerWalkSpeed;

	static const float playerSize;

	static const float playerBaseGravity;
	static const float playerFastGravity;

	static const float playerStartWorldLocationZ;
	static const float worldLocationProportionZ;

	// Immutable

	float m_currentWorldLocationZ;

	float m_movementX;
	float m_movementY;

	float m_currentMobility;
	float m_previousMobility;

	float m_currentSize;
	float m_previousSize;

//////

	bool m_grounded; // TODO should be read only

	FRotator m_controlRotation;
	FVector m_velocity; // TODO remove?

	float m_jumpZVelocity;

	bool m_shouldJump;

	PlayerBasics();

	float getJumpVelocity(float height);

	float getAnimationPoint(float x);

	void updateAttributes();

};
