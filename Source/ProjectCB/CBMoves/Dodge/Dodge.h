#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "Dodge_Hold.h"
#include "Dodge_Release.h"

class PROJECTCB_API Dodge // Dodge Dip Duck Dive Dodge
{
private:

	Dodge_Hold m_dodgeHold;
	Dodge_Release m_dodgeRelease;

	PlayerBasics* m_playerBasics;

public:

	Dodge(PlayerBasics& playerBasics);

	void onPress();

	void onRelease();

	void dodgeUpdate(float deltaTime);
};
