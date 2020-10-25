#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "Dodge_Hold.h"
#include "Dodge_Release.h"

class PROJECTCB_API Dodge // Dodge Dip Duck Dive Dodge
{
private:

	enum State { OFF = 0, START, DUCK, IDLE, JUMP, COOLDOWN } m_state;

	PlayerBasics* m_playerBasics;

	unsigned short m_frame;

	float m_diveProportion;

	bool m_buffer;

	float dodgeProportion(float dodgeValue, float diveValue);

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
