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
	if (!this->m_grabbedObject) // TODO should make it so that this can be removed? (make cooldown after grab?)
	{
		IGrabbable* topGrabbable = this->m_grabbableList->getTopGrabbable();

		if (topGrabbable)
		{
			topGrabbable->makeGrabbed();
			this->m_grabbedObject = topGrabbable;
			//this->m_grabbableList->remove(topGrabbable); // shouldn't remove?
		}
	}
}

void Throw::throwAimUpdate(float deltaTime)
{
	// TODO implement aim
}

void Throw::catchCooldownUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = PlayerBasics::THROW_OFF; // TODO implement cooldown
}

void Throw::throwCooldownUpdate(float deltaTime)
{
	this->m_playerBasics->m_throwState = PlayerBasics::THROW_OFF; // TODO implement cooldown
}