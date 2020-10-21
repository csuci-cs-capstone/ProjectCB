#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../BlendUpdater.h"

class PROJECTCB_API Dodge_Hold
{
	// TODO The additional height required by jump to be equal is equal to...
		// (StartSize - CurrentSize) / 2

private:

	//struct ActionUpdater : public BlendUpdater
	//{
	//private:

	//	Dodge_Hold& m_dodgeHold;

	//	void set(float deltaTime);

	//	void blend(float deltaTime, float amount);

	//public:

	//	ActionUpdater(Dodge_Hold& dodgeHold);
	//};

	//PlayerBasics& m_playerBasics;

public:

// Mutable

	static const float colliderSize;

	static const float startupMobility;
	static const float actionMobility;

	static const short startupFrames;
	static const short actionFrames;

// Immutable

	short m_frame;

	Dodge_Hold();

	void update(PlayerBasics& playerBasics);
};
