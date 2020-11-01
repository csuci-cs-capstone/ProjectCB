#include "Movement.h"
#include <math.h>
#include "../../CBMath/MathConstants.h"

const float Movement::playerWalkSpeed = 500.0f;
const float Movement::acceleration = 1.0f / 8;
const float Movement::deceleration = Movement::acceleration * 2;

Movement::Movement()
{
	this->m_inputVelocity.X = 0.0f;
	this->m_inputVelocity.Y = 0.0f;

	this->m_currentVelocity.X = 0.0f;
	this->m_currentVelocity.Y = 0.0f;

	this->m_playerRotation = FRotator(0.0f, 0.0f, 0.0f);
}

void Movement::updateVelocity()
{
	FVector2D diff = this->m_inputVelocity - this->m_currentVelocity;
	float diffMag = diff.Size();
	float accProp = FVector2D::DotProduct(diff.GetSafeNormal(), this->m_currentVelocity.GetSafeNormal());
	accProp = (accProp + 1) / 2;
	Proportion accProportion(accProp);
	float accAmount = accProportion.getProportion(Movement::acceleration, Movement::deceleration);
	if (accAmount >= diffMag)
		this->m_currentVelocity = this->m_inputVelocity;
	else
		this->m_currentVelocity += diff * (accAmount / diffMag);

	if (!this->m_currentVelocity.IsNearlyZero())
	{
		this->m_playerRotation.Yaw = MathConstants::RAD_TO_DEG
			* atan2f(this->m_currentVelocity.Y, this->m_currentVelocity.X);

		// TODO make take into account positive/negative values
	}
}

FRotator& Movement::getPlayerRotation()
{
	return this->m_playerRotation;
}
