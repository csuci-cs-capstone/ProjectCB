#pragma once

#include "CoreMinimal.h"
#include "../../CBMath/Proportion.h"

struct PROJECTCB_API Movement
{
	// TODO add other movement variables here

	static const float playerWalkSpeed;
	static const float acceleration; // between 0 and 1
	static const float deceleration; // between 0 and 1

private:

	FRotator m_playerRotation;

public:

	FVector2D m_inputVelocity;
	FVector2D m_currentVelocity;

	Movement();

	const FRotator& getPlayerRotation();

	void updateVelocity();
};
