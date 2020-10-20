#include "Dodge_Hold.h"

const float Dodge_Hold::colliderSize = 25.0f;

const float Dodge_Hold::startupMobility = 0.0f;
const float Dodge_Hold::actionMobility = 0.0f;

const short Dodge_Hold::startupFrames = 6;
const short Dodge_Hold::actionFrames = 24;

void Dodge_Hold::dodgeHoldUpdate(PlayerBasics playerBasics)
{
	if (this->m_frame == 1)
		playerBasics.m_previousSize = playerBasics.m_currentSize;

	if (this->m_frame >= Dodge_Hold::startupFrames)
	{
		if (this->m_frame >= (Dodge_Hold::startupFrames + Dodge_Hold::actionFrames))
		{
			playerBasics.m_mobility = 0; // TODO make variable

			playerBasics.m_currentSize = Dodge_Hold::colliderSize;
		}
		else
		{
			float proportion = (this->m_frame - Dodge_Hold::startupFrames) / (Dodge_Hold::actionFrames * 1.0f);

			proportion = playerBasics.getAnimationPoint(proportion);

			playerBasics.m_currentSize = (1 - proportion) * playerBasics.m_previousSize
				+ proportion * Dodge_Hold::colliderSize;
		}
	}

	this->m_frame++;
}