#pragma once

#include "CoreMinimal.h"

struct PROJECTCB_API Hold
{
	// Mutable
	const float m_colliderSize = 25.0f;

	const float m_startupMobility = 0.0f;
	const float m_actionMobility = 0.0f;

	const short m_startupFrames = 6;
	const short m_actionFrames = 24;

	// Immutable

	short m_frame;
};
