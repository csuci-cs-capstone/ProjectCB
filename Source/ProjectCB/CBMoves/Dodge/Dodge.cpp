#include "Dodge.h"

Dodge::Dodge(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
	this->m_hold.m_playerBasics = &playerBasics;
}

void Dodge::onPress()
{
	//if (!this->m_dodgeHold.m_frame)
	//	this->m_dodgeHold.m_frame = true;

	//if (!this->m_dodgeHold.isRunning())
	//	this->m_dodgeHold.start();

	if (!this->m_hold.m_state)
	{
		this->m_hold.m_state = Hold::START;
		this->m_hold.m_frame = true;
	}
}

void Dodge::onRelease()
{
	//if (this->m_dodgeHold.isRunning())
	//{
	//	if (!this->m_dodgeRelease.m_dodgeCooldownFrame && this->m_playerBasics->m_grounded)
	//	{
	//		this->m_dodgeRelease.m_dodgeFrame = true;

	//		const FRotator YawRotation(0, this->m_playerBasics->m_controlRotation.Yaw, 0);

	//		FVector direction = (this->m_playerBasics->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
	//			+ (this->m_playerBasics->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

	//		direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

	//		this->m_dodgeRelease.m_diveProportion = direction.Size();

	//		this->m_playerBasics->m_velocity = (this->m_dodgeRelease.m_diveProportion * Dodge_Release::diveHorizontalVelocity)
	//			* direction;

	//		this->m_playerBasics->m_jumpZVelocity = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeVelocity,
	//			Dodge_Release::diveVerticalVelocity);

	//		this->m_playerBasics->m_airControl = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeControl,
	//			Dodge_Release::diveControl);

	//		this->m_playerBasics->m_shouldJump = true;
	//	}

	//	this->m_dodgeHold.end();
	//}

	if (this->m_hold.m_state)
	{
		if (this->m_release.m_state != Release::COOLDOWN && this->m_playerBasics->m_grounded)
		{
			//this->m_dodgeRelease.m_dodgeFrame = true;

			this->m_release.m_state = Release::JUMP;
			this->m_release.m_dodgeFrame = true;

			const FRotator YawRotation(0, this->m_playerBasics->m_controlRotation.Yaw, 0);

			FVector direction = (this->m_playerBasics->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->m_playerBasics->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

			this->m_release.m_diveProportion = direction.Size();

			this->m_playerBasics->m_velocity = (this->m_release.m_diveProportion * Dodge_Release::diveHorizontalVelocity)
				* direction;

			this->m_playerBasics->m_jumpZVelocity = this->m_release.dodgeProportion(Dodge_Release::dodgeVelocity,
				Dodge_Release::diveVerticalVelocity);

			this->m_playerBasics->m_airControl = this->m_release.dodgeProportion(Dodge_Release::dodgeControl,
				Dodge_Release::diveControl);

			this->m_playerBasics->m_shouldJump = true;
		}

		this->m_hold.m_state = Hold::OFF;
		this->m_hold.m_frame = false;
	}


	//if(this->)
	//if (this->m_dodgeHold.m_frame)
	//{
	//	if (!this->m_dodgeRelease.m_dodgeCooldownFrame && this->m_playerBasics->m_grounded)
	//	{
	//		this->m_dodgeRelease.m_dodgeFrame = true;

	//		const FRotator YawRotation(0, this->m_playerBasics->m_controlRotation.Yaw, 0);

	//		FVector direction = (this->m_playerBasics->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
	//			+ (this->m_playerBasics->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

	//		direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

	//		this->m_dodgeRelease.m_diveProportion = direction.Size();

	//		this->m_playerBasics->m_velocity = (this->m_dodgeRelease.m_diveProportion * Dodge_Release::diveHorizontalVelocity)
	//			* direction;

	//		this->m_playerBasics->m_jumpZVelocity = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeVelocity,
	//			Dodge_Release::diveVerticalVelocity);

	//		this->m_playerBasics->m_airControl = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeControl,
	//			Dodge_Release::diveControl);

	//		this->m_playerBasics->m_shouldJump = true;
	//	}

	//	this->m_dodgeHold.m_frame = false;
	//}
}

void Dodge::Hold::update(float deltaTime)
{
	//this->m_startUpdater.update(deltaTime);

	//this->m_actionUpdater.update(deltaTime);

	if (this->m_frame == 1)
		this->m_playerBasics->updateAttributes();

	if (this->m_frame >= Dodge_Hold::startupFrames)
	{
		if (this->m_frame >= (Dodge_Hold::startupFrames + Dodge_Hold::actionFrames))
		{
			this->m_playerBasics->m_mobility = Dodge_Hold::actionMobility;

			this->m_playerBasics->m_currentSize = Dodge_Hold::colliderSize;
		}
		else
		{
			float proportion = (this->m_frame - Dodge_Hold::startupFrames) / (Dodge_Hold::actionFrames * 1.0f);

			proportion = this->m_playerBasics->getAnimationPoint(proportion);

			this->m_playerBasics->m_currentSize = (1 - proportion) * this->m_playerBasics->m_previousSize
				+ proportion * Dodge_Hold::colliderSize;
		}
	}

	this->m_frame++;
}

float Dodge::Release::dodgeProportion(float dodgeValue, float diveValue)
{
	return (1 - this->m_diveProportion) * dodgeValue + this->m_diveProportion * diveValue;
}

void Dodge::Release::dodgeActionUpdate(PlayerBasics& playerBasics)
{
	if (this->m_dodgeFrame == 1)
		playerBasics.m_previousSize = playerBasics.m_currentSize;

	playerBasics.m_mobility = 1; // TODO make variable

	float proportion = this->m_dodgeFrame / this->dodgeProportion(Dodge_Release::dodgeFramesToApex,
		Dodge_Release::diveFramesToApex);

	// TODO make rotate for dive

	if (proportion >= 1)
	{
		if (proportion >= 2)
		{
			playerBasics.m_currentSize = this->dodgeProportion(Dodge_Release::dodgeEndColliderSize,
				Dodge_Release::diveEndColliderSize);
		}
		else
		{
			this->m_state = Release::FALL;
			proportion = playerBasics.getAnimationPoint(proportion - 1);

			float apexColliderSize = this->dodgeProportion(Dodge_Release::dodgeApexColliderSize,
				Dodge_Release::diveApexColliderSize);

			float endColliderSize = this->dodgeProportion(Dodge_Release::dodgeEndColliderSize,
				Dodge_Release::diveEndColliderSize);

			playerBasics.m_currentSize = ((1 - proportion) * apexColliderSize) + (proportion * endColliderSize);
		}
	}
	else
	{
		proportion = playerBasics.getAnimationPoint(proportion);

		float apexColliderSize = this->dodgeProportion(Dodge_Release::dodgeApexColliderSize,
			Dodge_Release::diveApexColliderSize);

		playerBasics.m_currentSize = (1 - proportion) * playerBasics.m_previousSize + proportion * apexColliderSize;
	}

	if (playerBasics.m_grounded)
	{
		this->m_dodgeFrame = false;
		this->m_state = Release::COOLDOWN;
		this->m_dodgeCooldownFrame = true;
		playerBasics.m_previousSize = playerBasics.m_currentSize;
	}
	else
		this->m_dodgeFrame++;
}

void Dodge::Release::dodgeCooldownUpdate(PlayerBasics& playerBasics)
{
	playerBasics.m_mobility = 0; // TODO make variable

	short maxCooldownFrames = this->dodgeProportion(Dodge_Release::dodgeCooldownFrames, Dodge_Release::diveCooldownFrames);

	if (this->m_dodgeCooldownFrame >= maxCooldownFrames)
	{
		this->m_state = Release::OFF;
		this->m_dodgeCooldownFrame = false;

		playerBasics.m_currentSize = this->dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
			Dodge_Release::diveCooldownColliderSize);

		playerBasics.m_mobility = 1; // TODO make variable
	}
	else
	{
		this->m_dodgeCooldownFrame++;

		float proportion = playerBasics.getAnimationPoint(this->m_dodgeCooldownFrame / (maxCooldownFrames * 1.0f));

		float colliderSize = this->dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
			Dodge_Release::diveCooldownColliderSize);

		playerBasics.m_currentSize = ((1 - proportion) * playerBasics.m_previousSize) + (proportion * colliderSize);
	}
}

void Dodge::dodgeUpdate(float deltaTime)
{
	if (this->m_release.m_state == Release::JUMP || this->m_release.m_state == Release::FALL)
	{
		//if (this->m_dodgeHold.isRunning())
		//	this->m_dodgeHold.end();

		this->m_release.dodgeActionUpdate(*(this->m_playerBasics));
	}
	else if (this->m_release.m_state == Release::COOLDOWN)
	{
		//if (this->m_dodgeHold.isRunning())
		//	this->m_dodgeHold.end();

		this->m_release.dodgeCooldownUpdate(*(this->m_playerBasics));
	}
	else if (this->m_hold.m_state) // TODO make should start?
	//{
	//	if(!this->m_dodgeHold.isRunning())
	//		this->m_dodgeHold.start();
	//}

		this->m_hold.update(deltaTime);
}