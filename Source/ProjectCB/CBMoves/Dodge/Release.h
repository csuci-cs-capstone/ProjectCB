#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"

class PROJECTCB_API Release
{
public:

	// Dodge (Release)

	const float m_dodgeHeight = 1;
	const float m_dodgeControl = 0.0f;
	const float m_dodgeApexColliderSize = 25.0f;
	const float m_dodgeEndColliderSize = 50.0f;
	const float m_dodgeCooldownColliderSize = 50.f;

	const short m_dodgeCooldownFrames = 15;
	const short m_dodgeFramesToApex = 7;

	// Dive (Release + Direction)

	const float m_diveHeight = m_dodgeHeight / 2;
	const float m_diveHorizontalVelocity = 1.75f * g_playerWalkSpeed;
	const float m_diveControl = 0.0f;
	const float m_diveApexColliderSize = 25.0f;
	const float m_diveEndColliderSize = 50.0f;
	const float m_diveCooldownColliderSize = 50.f;

	const short m_diveCooldownFrames = 15;
	const short m_diveFramesToApex = 5;

	float m_diveProportion;

	// Immutable

	const float m_dodgeVelocity = sqrt(400000 * m_dodgeHeight * g_playerBaseGravity);
	const float m_diveVerticalVelocity = sqrt(400000 * m_diveHeight * g_playerBaseGravity);

	short m_dodgeFrame;
	short m_dodgeCooldownFrame;
};
