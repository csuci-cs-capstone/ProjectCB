#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "Dodge_Hold.h"
#include "Dodge_Release.h"

class PROJECTCB_API Dodge // Dodge Dip Duck Dive Dodge
{
private:

	//Dodge_Hold m_dodgeHold;
	//Dodge_Release m_dodgeRelease; // TODO remove

	enum State { OFF = 0, START, DUCK, IDLE, JUMP, FALL, COOLDOWN } m_state;

	PlayerBasics* m_playerBasics;

	unsigned short m_frame;

	float m_diveProportion;

	float dodgeProportion(float dodgeValue, float diveValue);

	void startDodge();

	void dodgeActionUpdate();

	void dodgeCooldownUpdate();

	void holdUpdate(float deltaTime);

public:

	Dodge(PlayerBasics& playerBasics);

	void onPress();

	void onRelease();

	void update(float deltaTime);
};
