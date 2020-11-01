#include "Movement.h"
#include <math.h>
#include "../../CBMath/MathConstants.h"

const float Movement::PLAYER_GROUND_SPEED = 500.0f;
const float Movement::PLAYER_AIR_SPEED = Movement::PLAYER_GROUND_SPEED * 1.75f;
const float Movement::PLAYER_ACCELERATION = 1.0f / 16.0f;
const float Movement::PLAYER_DECELERATION = Movement::PLAYER_ACCELERATION * 1.5f;

Movement::Movement()
{
	this->m_inputVelocity.X = 0.0f;
	this->m_inputVelocity.Y = 0.0f;

	this->m_currentVelocity.X = 0.0f;
	this->m_currentVelocity.Y = 0.0f;

	this->m_playerRotation = FRotator(0.0f, 0.0f, 0.0f);

	this->m_playerSpeed = Movement::PLAYER_GROUND_SPEED;
}

void Movement::updateVelocity(float mobility)
{
	FVector2D diff = this->m_inputVelocity - this->m_currentVelocity;
	//diff *= mobility; // TODO make accurate
	float diffMag = diff.Size();

	float accProp = FVector2D::DotProduct(diff.GetSafeNormal(), this->m_currentVelocity.GetSafeNormal());
	accProp = (accProp + 1) / 2;
	Proportion accProportion(accProp);

	float accAmount = accProportion.getProportion(Movement::PLAYER_ACCELERATION, Movement::PLAYER_DECELERATION);

	if (accAmount >= diffMag)
		this->m_currentVelocity = this->m_inputVelocity;
	else
		this->m_currentVelocity += diff * (accAmount / diffMag);

	if (!this->m_currentVelocity.IsNearlyZero())
		this->m_playerRotation.Yaw = MathConstants::RAD_TO_DEG
			* atan2f(this->m_currentVelocity.Y, this->m_currentVelocity.X);
}

const FRotator& Movement::getPlayerRotation()
{
	return this->m_playerRotation;
}

const float Movement::getSpeed()
{
	return this->m_playerSpeed;
}

void Movement::isGrounded(bool grounded)
{
	if (grounded)
		this->m_playerSpeed = Movement::PLAYER_GROUND_SPEED;
	else
		this->m_playerSpeed = Movement::PLAYER_AIR_SPEED;
}
