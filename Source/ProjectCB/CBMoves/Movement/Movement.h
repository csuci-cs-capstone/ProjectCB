#pragma once

#include "CoreMinimal.h"
#include "../../CBMath/Proportion.h"

struct PROJECTCB_API Movement
{
	// TODO add other movement variables here

	static const float PLAYER_GROUND_SPEED;
	static const float PLAYER_AIR_SPEED;
	static const float PLAYER_ACCELERATION; // between 0 and 1
	static const float PLAYER_DECELERATION; // between 0 and 1

private:

	FRotator m_playerRotation;

	float m_playerSpeed;

public:

	FVector2D m_inputVelocity;
	FVector2D m_currentVelocity;

	Movement();

	const FRotator& getPlayerRotation();

	void updateVelocity(float mobility);

	const float getSpeed();

	void isGrounded(bool grounded);

};
