#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../../CBGeneral/Grabbable.h"
#include "../../CBGeneral/GrabbableObject.h"
#include "../../CBGeneral/GrabbableList.h"
#include "Throw_Hold.h"
#include "Throw_Release.h"

class PROJECTCB_API Throw // Catch Aim Throw
{
private:

	PlayerBasics* m_playerBasics;
	IGrabbableObject* m_grabbedObject;
	FTransform m_grabTransform;

	void catchStartUpdate(float deltaTime);
	void throwStartUpdate(float deltaTime);

	void catchAimUpdate(float deltaTime);
	void throwAimUpdate(float deltaTime);

	void catchCooldownUpdate(float deltaTime);
	void throwCooldownUpdate(float deltaTime);

public:

	static const FVector THROW_DIRECTION;
	static const float GRAB_OFFSET;

	GrabbableList m_grabbableList;

	Throw(PlayerBasics& playerBasics);

	void onPress();
	void onRelease(FRotator playerRotation);
	void update(FVector playerPosition, FRotator playerRotation, float deltaTime);

	void drop();

	//NOTE
	//Can move the functionality to where you think it fits better but I put it the throw class for right now!
	void removeBall();
	void launchBall(FRotator playerRotation);
};
