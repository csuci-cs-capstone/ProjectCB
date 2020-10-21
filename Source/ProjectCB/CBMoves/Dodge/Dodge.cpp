#include "Dodge.h"

Dodge::Dodge(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
	this->m_dodgeHold.m_playerBasics = &playerBasics;
}

//void Dodge::set(PlayerBasics& playerBasics)
//{
//	this->m_dodgeHold.m_playerBasics = &playerBasics;
//}

void Dodge::onPress()
{
	if (!this->m_dodgeHold.m_frame)
		this->m_dodgeHold.m_frame = true;
}

void Dodge::onRelease()
{
	if (this->m_dodgeHold.m_frame)
	{
		if (!this->m_dodgeRelease.m_dodgeCooldownFrame && this->m_playerBasics->m_grounded)
		{
			this->m_dodgeRelease.m_dodgeFrame = true;

			const FRotator YawRotation(0, this->m_playerBasics->m_controlRotation.Yaw, 0);

			FVector direction = (this->m_playerBasics->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->m_playerBasics->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

			this->m_dodgeRelease.m_diveProportion = direction.Size();

			this->m_playerBasics->m_velocity = (this->m_dodgeRelease.m_diveProportion * Dodge_Release::diveHorizontalVelocity)
				* direction;

			this->m_playerBasics->m_jumpZVelocity = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeVelocity,
				Dodge_Release::diveVerticalVelocity);

			this->m_playerBasics->m_airControl = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeControl,
				Dodge_Release::diveControl);

			this->m_playerBasics->m_shouldJump = true;
		}

		this->m_dodgeHold.m_frame = false;
	}
}

void Dodge::dodgeUpdate(float deltaTime)
{
	if (this->m_dodgeRelease.m_dodgeFrame)
	{
		//if (this->m_dodgeHold.isRunning())
		//	this->m_dodgeHold.end();

		this->m_dodgeRelease.dodgeActionUpdate(*(this->m_playerBasics), this->m_playerBasics->m_grounded);
	}
	else if (this->m_dodgeRelease.m_dodgeCooldownFrame)
	{
		//if (this->m_dodgeHold.isRunning())
		//	this->m_dodgeHold.end();

		this->m_dodgeRelease.dodgeCooldownUpdate(*(this->m_playerBasics));
	}
	else if (this->m_dodgeHold.m_frame) // TODO make should start?
	//{
	//	if(!this->m_dodgeHold.isRunning())
	//		this->m_dodgeHold.start();
	//}

		this->m_dodgeHold.update(deltaTime);
}