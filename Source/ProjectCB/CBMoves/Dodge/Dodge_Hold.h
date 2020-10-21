#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../BlendUpdater.h"

class PROJECTCB_API Dodge_Hold
{
	// TODO The additional height required by jump to be equal is equal to...
		// (StartSize - CurrentSize) / 2

private:

	struct ActionUpdater : public BlendUpdater
	{
		Dodge_Hold* const m_dodgeHold;

		ActionUpdater(Dodge_Hold* const dodgeHold, unsigned short totalFrames);

		void set(float deltaTime);

		void blend(float deltaTime, float amount);
	};
	

	//struct ActionUpdater : public BlendUpdater
	//{
	//private:

	//	void set(float deltaTime);

	//	void blend(float deltaTime, float amount);

	//public:

	//	Dodge_Hold* m_dodgeHold;
	//};

public:

// Mutable

	static const float colliderSize;

	static const float startupMobility;
	static const float actionMobility;

	static const short startupFrames;
	static const short actionFrames;

// Immutable

	ActionUpdater m_actionUpdater = ActionUpdater(this, 10);

	PlayerBasics* m_playerBasics;

	short m_frame;

	Dodge_Hold();

	void update();
};
