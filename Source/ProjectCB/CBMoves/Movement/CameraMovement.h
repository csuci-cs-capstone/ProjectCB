#pragma once

#include "CoreMinimal.h"

struct PROJECTCB_API CameraMovement
{
	static const FRotator CAMERA_START_ROTATION;
	static const float CAMERA_TURN_SPEED;
	static const float CAMERA_TURN_ACCELERATION; // between 0 and 1

private:

	FRotator m_cameraRotation;
	float m_velocity;

public:

	CameraMovement();

	void updateCamera(const float amount);

	const FRotator& getCameraRotation();
};
