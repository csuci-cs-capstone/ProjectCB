#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../../CBMath/Proportion.h"
#include "Dodge_Hold.h"
#include "Dodge_Release.h"

class PROJECTCB_API Dodge // Dodge Dip Duck Dive Dodge
{
private:

	PlayerBasics* m_playerBasics;

	unsigned short m_frame;

	Proportion m_dodgeProportion;

	bool m_buffer;

	void startDuck();
	void startDodge();

	void startUpdate(float deltaTime);
	void duckUpdate(float deltaTime);
	void idleUpdate(float deltaTime);
	void jumpUpdate(float deltaTime);
	void cooldownUpdate(float deltaTime);

public:

	Dodge(PlayerBasics& playerBasics);

	void onPress();
	void onRelease();
	void update(float deltaTime);
};
