#pragma once

#include "CoreMinimal.h"

struct PROJECTCB_API Dodge_Release
{
	// Dodge (Release)

	static const float dodgeHeight;
	static const float dodgeMobility;
	static const float dodgeApexColliderSize;
	static const float dodgeEndColliderSize;
	static const float dodgeCooldownMobility;
	static const float dodgeCooldownColliderSize;

	static const short dodgeCooldownFrames;
	static const short dodgeFramesToApex;

	// Dive (Release + Direction)

	static const float diveHeight;
	static const float diveHorizontalVelocity;
	static const float diveMobility;
	static const float diveApexColliderSize;
	static const float diveEndColliderSize;
	static const float diveCooldownMobility;
	static const float diveCooldownColliderSize;

	static const short diveCooldownFrames;
	static const short diveFramesToApex;
};
