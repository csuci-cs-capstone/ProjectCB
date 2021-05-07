#include "Throw_Hold.h"
#include "Throw.h"

void UThrow::catchStartUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = FPlayerBasics::CATCH_AIM; // TODO implement startup
}

void UThrow::throwStartUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = FPlayerBasics::THROW_AIM; // TODO implement startup
}

// TODO add to release

void UThrow::catchAimUpdate(float deltaTime)
{
	if (!this->m_grabbedObject)
	{
		IGrabbable* topGrabbable = this->m_grabbableList.getTopGrabbable();
		IGrabbableObject* topGrabbableObject = topGrabbable ? topGrabbable->getGrabbableObject() : nullptr;

		if (topGrabbableObject)
		{
			topGrabbableObject->makeGrabbed();
			this->m_grabbedObject = topGrabbableObject;
		}
	}
	else
		this->m_playerBasics->m_currentRadius = FPlayerBasics::PLAYER_RADIUS + this->m_grabbedObject->getRadius()
			+ (UThrow::GRAB_OFFSET / 2.0f);
}

void UThrow::throwAimUpdate(float deltaTime)
{
	// TODO implement aim
}