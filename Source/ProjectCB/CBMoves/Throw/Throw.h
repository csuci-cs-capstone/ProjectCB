#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "Throw_Hold.h"
#include "Throw_Release.h"

class PROJECTCB_API Throw // Catch Aim Throw
{
private:

	PlayerBasics* m_playerBasics;
	AActor* m_grabbedObject;

	void catchStartUpdate(float deltaTime);
	void throwStartUpdate(float deltaTime);

	void catchAimUpdate(float deltaTime);
	void throwAimUpdate(float deltaTime);

	void catchCooldownUpdate(float deltaTime);
	void throwCooldownUpdate(float deltaTime);

public:

	static const FVector THROW_DIRECTION;

	static bool isGrabbable(AActor* actor);

	AActor* m_grabbableObject;

	Throw(PlayerBasics& playerBasics);

	void onPress();
	void onRelease();
	void update(float deltaTime);
};
