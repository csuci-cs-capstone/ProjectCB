#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"

struct PROJECTCB_API Dodge_Release
{
	// Dodge (Release)

	static const float dodgeHeight;
	static const float dodgeControl;
	static const float dodgeApexColliderSize;
	static const float dodgeEndColliderSize;
	static const float dodgeCooldownColliderSize;

	static const short dodgeCooldownFrames;
	static const short dodgeFramesToApex;

	// Dive (Release + Direction)

	static const float diveHeight;
	static const float diveHorizontalVelocity;
	static const float diveControl;
	static const float diveApexColliderSize;
	static const float diveEndColliderSize;
	static const float diveCooldownColliderSize;

	static const short diveCooldownFrames;
	static const short diveFramesToApex;

	// Immutable

	static const float dodgeVelocity;
	static const float diveVerticalVelocity;

	float m_diveProportion;

	short m_dodgeFrame;
	short m_dodgeCooldownFrame;

	Dodge_Release();

	inline float dodgeProportion(float dodgeValue, float diveValue);

	void dodgeActionUpdate(PlayerBasics& playerBasics, bool grounded);

	void dodgeCooldownUpdate(PlayerBasics& playerBasics);
};
