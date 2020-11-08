#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"

class PROJECTCB_API Throw // Catch Aim Throw
{
private:

	PlayerBasics* m_playerBasics;
	AActor* m_grabbedObject;

public:

	static const FVector THROW_DIRECTION;

	AActor* m_grabbableObject;

	Throw(PlayerBasics& playerBasics);

	void onPress();
	void onRelease();
	void update(float deltaTime);
};
