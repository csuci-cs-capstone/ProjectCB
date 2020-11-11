#include "Throw_Hold.h"
#include "Throw.h"

void Throw::catchStartUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = PlayerBasics::CATCH_AIM; // TODO implement startup
}

void Throw::throwStartUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = PlayerBasics::THROW_AIM; // TODO implement startup
}

// TODO add to release

void Throw::catchAimUpdate(float deltaTime)
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
}

void Throw::throwAimUpdate(float deltaTime)
{
	// TODO implement aim
}