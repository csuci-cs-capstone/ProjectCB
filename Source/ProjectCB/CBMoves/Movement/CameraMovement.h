#pragma once

#include "CoreMinimal.h"
#include "Movement.h"

struct PROJECTCB_API CameraMovement
{
	static const FRotator CAMERA_START_ROTATION;
	static const float CAMERA_TURN_SPEED;
	static const float CAMERA_TURN_ACCELERATION; // between 0 and 1

private:

	FRotator m_cameraRotation;
	float m_velocity;
	float m_sensitivity;

public:

	CameraMovement();

	void updateCamera(Movement& movement, const float amount);

	void setCameraSensitivity(float cameraSensitivity);

	const FRotator& getCameraRotation();
};
