#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"

struct PROJECTCB_API Dodge_Hold
{
	// TODO The additional height required by jump to be equal is equal to...
		// (StartSize - CurrentSize) / 2

	// Mutable
	static const float colliderSize;

	static const float startupMobility;
	static const float actionMobility;

	static const short startupFrames;
	static const short actionFrames;

	// Immutable

	short m_frame;

	void dodgeHoldUpdate(PlayerBasics playerBasics);
};
