#include "CameraMovement.h"

const FRotator CameraMovement::startRotation(-20.0f, 0.0f, 0.0f);
const float CameraMovement::maxTurnSpeed = 1.0f * 2;
const float CameraMovement::acceleration = 1.0f / 8;

CameraMovement::CameraMovement()
{
	this->m_cameraRotation = CameraMovement::startRotation;
	this->m_velocity = 0;
}

void CameraMovement::updateCamera(const float amount)
{
	if (amount == 0) // TODO check for threshold
	{
		this->m_velocity = 0;
	}
	else if (amount < 0)
	{
		if (this->m_velocity > 0)
			this->m_velocity = 0;

		this->m_velocity += CameraMovement::acceleration * amount;

		if (this->m_velocity < -1)
			this->m_velocity = -1;
	}
	else
	{
		if (this->m_velocity < 0)
			this->m_velocity = 0;

		this->m_velocity += CameraMovement::acceleration * amount;

		if (this->m_velocity > 1)
			this->m_velocity = 1;
	}

	this->m_cameraRotation.Yaw += CameraMovement::maxTurnSpeed * this->m_velocity;

	if (this->m_cameraRotation.Yaw >= 360)
		this->m_cameraRotation.Yaw -= 360;
	else if (this->m_cameraRotation.Yaw < 0)
		this->m_cameraRotation.Yaw += 360;
}

const FRotator& CameraMovement::getCameraRotation()
{
	return this->m_cameraRotation;
}