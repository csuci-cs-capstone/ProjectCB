#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../BoundedUpdater.h"

class PROJECTCB_API Dodge_Hold
{
	// TODO The additional height required by jump to be equal is equal to...
		// (StartSize - CurrentSize) / 2

private:

	struct StartUpdater : public BoundedUpdater
	{
		Dodge_Hold* const m_dodgeHold;

		StartUpdater(Dodge_Hold* const dodgeHold, unsigned short totalFrames);

		void onStart();

		void onEnd();

		void action(float deltaTime, float amount);
	};

	struct ActionUpdater : public BoundedUpdater
	{
		Dodge_Hold* const m_dodgeHold;

		ActionUpdater(Dodge_Hold* const dodgeHold, unsigned short totalFrames);

		void onEnd();

		void action(float deltaTime, float amount);
	};

	StartUpdater m_startUpdater = StartUpdater(this, 10);
	ActionUpdater m_actionUpdater = ActionUpdater(this, 10);

public:

// Mutable

	static const float colliderSize;

	static const float startupMobility;
	static const float actionMobility;

	static const short startupFrames;
	static const short actionFrames;

// Immutable

	PlayerBasics* m_playerBasics;

	short m_frame;

	Dodge_Hold();

	bool isRunning();

	void start();

	void end();

	void update(float deltaTime);
};
