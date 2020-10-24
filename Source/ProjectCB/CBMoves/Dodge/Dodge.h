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

	PlayerBasics* m_playerBasics;

	struct Hold
	{
		enum State { OFF = 0, START, DUCK, IDLE } m_state;

		PlayerBasics* m_playerBasics;

		unsigned short m_frame;

		void update(float deltaTime);
	};

	struct Release
	{
		enum State { OFF = 0, JUMP, FALL, COOLDOWN } m_state;

		unsigned short m_dodgeFrame, m_dodgeCooldownFrame;

		float m_diveProportion;

		float dodgeProportion(float dodgeValue, float diveValue);

		void dodgeActionUpdate(PlayerBasics& playerBasics);

		void dodgeCooldownUpdate(PlayerBasics& playerBasics);
	};

	Hold m_hold;
	Release m_release;

	//enum State { OFF = 0, HOLD_START, HOLD_ACTION, HOLD_END } m_state;

public:

	Dodge(PlayerBasics& playerBasics);

	void onPress();

	void onRelease();

	void dodgeUpdate(float deltaTime);
};
