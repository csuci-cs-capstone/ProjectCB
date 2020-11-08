#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../../CBGeneral/Grabbable.h"
#include "Throw_Hold.h"
#include "Throw_Release.h"

class PROJECTCB_API Throw // Catch Aim Throw
{
private:

	PlayerBasics* m_playerBasics;
	IGrabbable* m_grabbedObject;

	void catchStartUpdate(float deltaTime);
	void throwStartUpdate(float deltaTime);

	void catchAimUpdate(float deltaTime);
	void throwAimUpdate(float deltaTime);

	void catchCooldownUpdate(float deltaTime);
	void throwCooldownUpdate(float deltaTime);

public:

	static const FVector THROW_DIRECTION;

	IGrabbable* m_grabbableObject;

	Throw(PlayerBasics& playerBasics);

	void onPress();
	void onRelease();
	void update(FVector playerPosition, FRotator playerRotation, float deltaTime);
};
