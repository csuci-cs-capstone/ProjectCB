#include "CameraMovement.h"
#include "Net/UnrealNetwork.h"

const FRotator CameraMovement::CAMERA_START_ROTATION(-20.0f, 0.0f, 0.0f);
const float CameraMovement::CAMERA_TURN_SPEED = 1.0f * 2;
const float CameraMovement::CAMERA_TURN_ACCELERATION = 1.0f / 8;

CameraMovement::CameraMovement()
{
	this->m_cameraRotation = CameraMovement::CAMERA_START_ROTATION;
	this->m_velocity = 0;
}

void CameraMovement::updateCamera(const float amount)
{
	if (amount == 0)
		this->m_velocity = 0;
	else if (amount < 0)
	{
		if (this->m_velocity > 0)
			this->m_velocity = 0;

		this->m_velocity += CameraMovement::CAMERA_TURN_ACCELERATION * amount;

		if (this->m_velocity < -1)
			this->m_velocity = -1;
	}
	else
	{
		if (this->m_velocity < 0)
			this->m_velocity = 0;

		this->m_velocity += CameraMovement::CAMERA_TURN_ACCELERATION * amount;

		if (this->m_velocity > 1)
			this->m_velocity = 1;
	}

	this->m_cameraRotation.Yaw += CameraMovement::CAMERA_TURN_SPEED * this->m_velocity;

	if (this->m_cameraRotation.Yaw >= 360)
		this->m_cameraRotation.Yaw -= 360;
	else if (this->m_cameraRotation.Yaw < 0)
		this->m_cameraRotation.Yaw += 360;
}

const FRotator& CameraMovement::getCameraRotation()
{
	return this->m_cameraRotation;
}

void CameraMovement::SetRotationBasedOnTeam(FString TeamName)
{
	if (TeamName == "yellow")
	{
		this->m_cameraStart = FRotator(-20.0f, 0.0f, 108.0f);
	}
	else
	{
		this->m_cameraStart = FRotator(-20.0f, 0.0f, 0.0f);
	}
}
