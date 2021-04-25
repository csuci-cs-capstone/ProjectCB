#pragma once

#include "CoreMinimal.h"
#include "../../CBPlayer/PlayerBasics.h"
#include "../../CBGeneral/Grabbable.h"
#include "../../CBGeneral/GrabbableObject.h"
#include "../../CBGeneral/GrabbableList.h"
#include "Net/UnrealNetwork.h"
#include "Throw_Hold.h"
#include "Throw_Release.h"
#include "Throw.generated.h"

UCLASS()
class PROJECTCB_API UThrow : public UObject// Catch Aim Throw
{

	GENERATED_BODY()

private:

	FPlayerBasics* m_playerBasics;
	IGrabbableObject* m_grabbedObject;
	

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
	FTransform m_grabTransform;

	UThrow();

	void setPlayerBasics(FPlayerBasics& playerBasics);

	UFUNCTION(Client, Reliable, WithValidation)
	void onPress();
	UFUNCTION(Client, Reliable, WithValidation)
	void onRelease(FRotator playerRotation);
	UFUNCTION(Client, Reliable, WithValidation)
	void update(FVector playerPosition, FRotator playerRotation, float deltaTime);
	UFUNCTION(Client, Reliable, WithValidation)
	void drop();

	//NOTE
	//Can move the functionality to where you think it fits better but I put it the throw class for right now!
	UFUNCTION(Client, Reliable, WithValidation)
	void removeBall();
	UFUNCTION(Server, Reliable)
	void launchBall(FRotator playerRotation);
	
	void updateCurrentlyGrabbed(IGrabbableObject* currentGrabbedObject);

protected:

	bool IsNameStableForNetworking() const override;
};
