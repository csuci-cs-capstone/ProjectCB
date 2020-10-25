#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../BoundedUpdater.h"

class PROJECTCB_API Dodge_Hold
{
	// TODO The additional height required by jump to be equal is equal to...
		// (StartSize - CurrentSize) / 2

public:

	static const float mobility;
	static const float colliderSize;
	
	static const short startupFrames;
	static const short actionFrames;
};
