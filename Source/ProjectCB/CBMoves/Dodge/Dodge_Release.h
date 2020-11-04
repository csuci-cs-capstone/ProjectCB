#pragma once

#include "CoreMinimal.h"

struct PROJECTCB_API Dodge_Release
{
	// Dodge (Release)

	static const float DODGE_HEIGHT;
	static const float DODGE_MOBILITY;
	static const float DODGE_APEX_COLLIDER_SIZE;
	static const float DODGE_END_COLLIDER_SIZE;
	static const float DODGE_LAND_VELOCITY_RESET_PROPORTION;
	static const float DODGE_COOLDOWN_MOBILITY;

	static const short DODGE_FRAMES_TO_APEX;
	static const short DODGE_COOLDOWN_FRAMES;

	// Dive (Release + Direction)

	static const float DIVE_HEIGHT;
	static const float DIVE_MOBILITY;
	static const float DIVE_APEX_COLLIDER_SIZE;
	static const float DIVE_END_COLLIDER_SIZE;
	static const float DIVE_LAND_VELOCITY_RESET_PROPORTION;
	static const float DIVE_COOLDOWN_MOBILITY;

	static const short DIVE_FRAMES_TO_APEX;
	static const short DIVE_COOLDOWN_FRAMES;
	
};
