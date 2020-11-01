#pragma once

#include "CoreMinimal.h"

struct PROJECTCB_API CameraMovement
{
	static const FRotator startRotation;
	static const float maxTurnSpeed;
	static const float acceleration; // between 0 and 1

private:

	FRotator m_cameraRotation;
	float m_velocity;

public:

	CameraMovement();

	void updateCamera(const float amount);

	const FRotator& getCameraRotation();
};
