#include "Movement.h"
#include <math.h>
#include "../../CBMath/MathConstants.h"

// TODO add GHOST_SPEED;

const float Movement::PLAYER_GROUND_SPEED = 1000.0f;
const float Movement::PLAYER_AIR_SPEED = Movement::PLAYER_GROUND_SPEED * 1.75f;
const float Movement::PLAYER_ACCELERATION = 1.0f / 16.0f;
const float Movement::PLAYER_DECELERATION = Movement::PLAYER_ACCELERATION * 1.5f;

const float Movement::GHOST_SPEED = Movement::PLAYER_GROUND_SPEED * 0.75f;
const float Movement::GHOST_ACCELERATION = Movement::PLAYER_ACCELERATION;
const float Movement::GHOST_DECELERATION = Movement::PLAYER_DECELERATION;

void Movement::updateVelocity(FVector2D& currentVelocity, float mobility)
{
	FVector2D diff = this->m_inputVelocity - currentVelocity;
	float diffMag = diff.Size();

	float accProp = FVector2D::DotProduct(diff.GetSafeNormal(), currentVelocity.GetSafeNormal());
	accProp = (accProp + 1) / 2;
	Proportion accProportion(accProp);

	float accAmount = mobility * accProportion.getProportion(Movement::PLAYER_ACCELERATION, Movement::PLAYER_DECELERATION);

	if (accAmount != 0)
	{
		if (accAmount >= diffMag)
			currentVelocity = this->m_inputVelocity;
		else
			currentVelocity += diff * (accAmount / diffMag);
	}
}

Movement::Movement()
{
	this->m_inputVelocity.X = 0.0f;
	this->m_inputVelocity.Y = 0.0f;

	this->m_currentMovementVelocity.X = 0.0f;
	this->m_currentMovementVelocity.Y = 0.0f;

	this->m_currentRotationVelocity.X = 0.0f;
	this->m_currentRotationVelocity.Y = 0.0f;

	this->m_baseVelocity.X = 0.0f;
	this->m_baseVelocity.Y = 0.0f;

	this->m_inAir = false;

	this->m_startRotation = FRotator(0.0f, 0.0f, 0.0f);
	this->m_playerRotation = FRotator(0.0f, 0.0f, 0.0f);

	this->m_playerSpeed = Movement::PLAYER_GROUND_SPEED;
}

void Movement::updateVelocity(float mobility)
{
	if (this->m_inputVelocity.Size() > 1)
		this->m_inputVelocity.Normalize();

	updateVelocity(this->m_currentMovementVelocity, mobility);
	updateVelocity(this->m_currentRotationVelocity, 1);

	if (!this->m_currentRotationVelocity.IsNearlyZero())
		this->m_playerRotation.Yaw = MathConstants::RAD_TO_DEG
		* atan2f(this->m_currentRotationVelocity.Y, this->m_currentRotationVelocity.X);
}

void Movement::setStartRotation(FRotator startRotation)
{
	this->m_startRotation = startRotation;
}

void Movement::setRotation(FRotator rotation)
{
	this->m_playerRotation = rotation - this->m_startRotation;
}

FRotator Movement::getPlayerRotation()
{
	return this->m_startRotation + this->m_playerRotation;
}

void Movement::isGrounded(bool grounded, bool isGhost)
{
	if (grounded)
	{
		this->m_playerSpeed = Movement::PLAYER_GROUND_SPEED;

		if (this->m_inAir)
		{
			this->m_baseVelocity.X = 0.0f;
			this->m_baseVelocity.Y = 0.0f;

			this->m_inAir = false;
		}
	}
	else
	{
		this->m_playerSpeed = Movement::PLAYER_AIR_SPEED;
		this->m_inAir = true;
	}

	if (isGhost)
		this->m_playerSpeed = Movement::PLAYER_GROUND_SPEED;
}

void Movement::setInputRotation(float inputRotationYaw)
{
	this->m_inputRotationYaw = inputRotationYaw;
}

FVector Movement::getInputVector(float cameraRotationYaw)
{
	const FRotator YawRotation(0, this->m_inputRotationYaw + cameraRotationYaw, 0);

	return (this->m_inputVelocity.X * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)) +
		(this->m_inputVelocity.Y * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
}

FVector Movement::getMovementVelocity(float velocityZ)
{
	return FVector(this->m_playerSpeed * (this->m_currentMovementVelocity + this->m_baseVelocity), velocityZ);
}

void Movement::resetInputVelocity()
{
	this->m_inputVelocity.X = 0.0f;
	this->m_inputVelocity.Y = 0.0f;
}

void Movement::addInputVector(const FVector& inputVector)
{
	this->m_inputVelocity.X += inputVector.X;
	this->m_inputVelocity.Y += inputVector.Y;
}

void Movement::resetMovement(float amount)
{
	this->m_currentMovementVelocity *= 1.0f - amount;
}

void Movement::setMovementVelocity(const FVector& velocity, const FVector& baseVelocity)
{
	this->m_inputVelocity.X = velocity.X;
	this->m_inputVelocity.Y = velocity.Y;

	this->m_currentMovementVelocity.X = velocity.X;
	this->m_currentMovementVelocity.Y = velocity.Y;

	this->m_baseVelocity.X = baseVelocity.X;
	this->m_baseVelocity.Y = baseVelocity.Y;
}