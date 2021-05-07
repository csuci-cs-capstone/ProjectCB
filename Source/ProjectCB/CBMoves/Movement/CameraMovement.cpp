#include "CameraMovement.h"
#include "Net/UnrealNetwork.h"

const FRotator CameraMovement::CAMERA_START_ROTATION(-20.0f, 0.0f, 0.0f);
const float CameraMovement::CAMERA_TURN_SPEED = 1.0f * 1.5f;
const float CameraMovement::CAMERA_TURN_ACCELERATION = 1.0f / 8;

CameraMovement::CameraMovement()
{
	this->m_cameraRotation = CameraMovement::CAMERA_START_ROTATION;
	this->m_velocity = 0;
	this->m_sensitivity = 1.0;
}

void CameraMovement::updateCamera(Movement& movement, const float amount)
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

	this->m_cameraRotation.Yaw += this->m_sensitivity * CameraMovement::CAMERA_TURN_SPEED * this->m_velocity;

	if (this->m_cameraRotation.Yaw >= 360)
		this->m_cameraRotation.Yaw -= 360;
	else if (this->m_cameraRotation.Yaw < 0)
		this->m_cameraRotation.Yaw += 360;

	movement.setCameraRotation(this->m_cameraRotation, this->m_velocity != 0);
}

void CameraMovement::setCameraSensitivity(float cameraSensitivity)
{
	this->m_sensitivity = cameraSensitivity;
}

const FRotator& CameraMovement::getCameraRotation()
{
	return this->m_cameraRotation;
}

void CameraMovement::SetRotationBasedOnTeam(FString TeamName)
{
	if (TeamName == "yellow")
	{
		this->m_cameraStart = FRotator(-20.0f, 0.0f, 180.0f);
	}
	else
	{
		this->m_cameraStart = FRotator(-20.0f, 0.0f, 0.0f);
	}
}
