#pragma once

#include "CoreMinimal.h"

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

	// TODO 'fix' the camera at the original center of character (based on character size) [smoothly update]
		// make based on map? make off-center for better visualation of map and balls
		// make player transparent?

	float m_currentWorldLocationZ;

	float m_movementX;
	float m_movementY;
	float m_mobility;

	float m_currentSize;
	float m_previousSize;

//////

	bool m_grounded; // TODO should be read only

	FRotator m_controlRotation; // TODO should be read only

	// TODO have a method for updating velocity (etc.)
	FVector m_velocity;

	float m_jumpZVelocity;
	float m_airControl;

	bool m_shouldJump;

	PlayerBasics();

	float getAnimationPoint(float x);

	void updateAttributes();

};
