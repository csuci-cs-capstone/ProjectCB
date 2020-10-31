#pragma once

#include "CoreMinimal.h"

class PROJECTCB_API CameraMovement
{
private:

	static const float acceleration; // between 0 and 1
	static const float deceleration; // between 0 and 1

public:

	CameraMovement();

	~CameraMovement();

};
