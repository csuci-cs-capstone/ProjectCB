#pragma once

#include "CoreMinimal.h"

struct PROJECTCB_API PlayerBasics
{
	// Mutable

	static const float playerWalkSpeed;
	static const float playerBaseGravity;
	static const float playerFastGravity;
	static const float playerStartWorldLocationZ;
	static const float worldLocationProportionZ;

	// Immutable

	// TODO 'fix' the camera at the original center of character (based on character size) [smoothly update]
		// make based on map? make off-center for better visualation of map and balls
		// make player transparent?

	float m_currentWorldLocationZ;

	float m_movementX;
	float m_movementY;
	float m_mobility;

	float m_currentSize = 50.0f;
	float m_previousSize;

	PlayerBasics();

	float getAnimationPoint(float x);
};
