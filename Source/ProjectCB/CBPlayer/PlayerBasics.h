#pragma once

#include "CoreMinimal.h"
#include <math.h>
#include "../CBMoves/Movement/Movement.h"
#include "../CBMoves/Movement/CameraMovement.h"

struct PROJECTCB_API PlayerBasics
{
	// Mutable

	static const float playerSize;

	static const float playerBaseGravity;
	static const float playerFastGravity;

	static const float playerStartWorldLocationZ;
	static const float worldLocationProportionZ;

	// Immutable

	enum PlayerState { PLAYER_ALIVE = 0, PLAYER_GHOST } m_playerState;
	enum DodgeState { DODGE_OFF = 0, DODGE_STARTUP, DODGE_DUCK, DODGE_IDLE, DODGE_JUMP, DODGE_COOLDOWN } m_dodgeState;
	enum ThrowState { THROW_OFF = 0, THROW_STARTUP, THROW_CATCH, THROW_CATCH_IDLE, THROW_THROW, THROW_COOLDOWN } m_throwState;

	float m_currentWorldLocationZ;

	float m_movementX;
	float m_movementY;

	float m_currentMobility;
	float m_previousMobility;

	float m_currentSize;
	float m_previousSize;

//////

	Movement m_movement;
	CameraMovement m_cameraMovement;

/////

	bool m_grounded; // TODO should be read only

	FRotator m_controlRotation;
	FVector m_velocity; // TODO remove?

	float m_jumpZVelocity;

	bool m_shouldJump;

	PlayerBasics();

	float getJumpVelocity(float height);

	float getAnimationPoint(float x);

	void updateAttributes();

};
