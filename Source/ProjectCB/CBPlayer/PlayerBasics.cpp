#include "PlayerBasics.h"

const FVector2D PlayerBasics::MAX_MAP_POSITION(1475.0f, 975.0f);
const float PlayerBasics::MAP_RESPAWN_POSITION_OFFSET = 50.0f;
const float PlayerBasics::MIN_MAP_POSITION_Z = -200.0f;

const float PlayerBasics::PLAYER_RADIUS = 25.0f;
const float PlayerBasics::PLAYER_HEIGHT = 50.0f;

const float PlayerBasics::PLAYER_SPAWN_POSITION_Z = 100.0f + PlayerBasics::PLAYER_HEIGHT;

const float PlayerBasics::PLAYER_BASE_GRAVITY = 4.0f;
const float PlayerBasics::PLAYER_FAST_GRAVITY = 1.5f * PlayerBasics::PLAYER_BASE_GRAVITY;

const float PlayerBasics::PLAYER_START_WORLD_LOCATION_Z = 250.0f;
const float PlayerBasics::WORLD_LOCATION_PROPORTION_Z = 0.75f;

const float PlayerBasics::LAUNCH_SPEED = 100.0f;
const float PlayerBasics::LAUNCH_MOBILITY = 0.25f;
const float PlayerBasics::LAUNCH_HEIGHT = 50.0f;

const short PlayerBasics::RESET_COLLISION_FRAMES = 3; // TODO remove?

PlayerBasics::PlayerBasics()
{
	this->m_playerState = PLAYER_ALIVE;
	this->m_dodgeState = DODGE_OFF;
	this->m_throwState = THROW_OFF;

	this->m_currentWorldLocationZ = PlayerBasics::PLAYER_START_WORLD_LOCATION_Z;

	this->m_currentMobility = 1.0f;
	this->m_previousMobility = this->m_currentMobility;

	this->m_currentRadius = PlayerBasics::PLAYER_RADIUS;

	this->m_currentHeight = PlayerBasics::PLAYER_HEIGHT;
	this->m_previousHeight = this->m_currentHeight;

	this->m_jumpZVelocity = 0;

	this->m_shouldJump = false;
	this->m_fellOff = false;
}

float PlayerBasics::getJumpVelocity(float height)
{
	return sqrt(5000 * (height + ((PlayerBasics::PLAYER_HEIGHT - this->m_currentHeight) / 20.0f))
		* PlayerBasics::PLAYER_BASE_GRAVITY);
}

float PlayerBasics::getAnimationPoint(float x)
{
	return x; // TODO adjust to sync with procedural animations (add additional parameters where needed)
}

void PlayerBasics::updateAttributes()
{
	this->m_previousMobility = this->m_currentMobility;
	this->m_previousHeight = this->m_currentHeight;
}

void PlayerBasics::updateGroundState(bool grounded)
{
	this->m_grounded = grounded;

	if(this->m_grounded)
		this->m_currentMobility = 1.0f;

	this->m_movement.isGrounded(this->m_grounded,
		this->m_playerState == PlayerBasics::PlayerState::PLAYER_GHOST);
}

bool PlayerBasics::isGrounded()
{
	return this->m_grounded;
}

PlayerBasics::PlayerState PlayerBasics::getPlayerState()
{
	return this->m_playerState;
}

void PlayerBasics::makeGhost()
{
	this->m_playerState = PlayerBasics::PLAYER_GHOST;
	this->m_dodgeState = PlayerBasics::DODGE_OFF;
	this->m_throwState = PlayerBasics::THROW_OFF;

	this->m_currentMobility = 1.0f;
	this->m_currentHeight = PlayerBasics::PLAYER_HEIGHT;

	// DROP GRABBED ITEM

	//Model/Anims
	m_playerSkeletalMeshComponent->SetSkeletalMesh(m_ghostModel);

	updateAttributes();

	// TODO update other attributes for ghost such as GHOST_SPEED
}

FVector PlayerBasics::checkPlayerBounds(FVector playerPosition)
{
	switch (getPlayerState())
	{
		case PlayerBasics::PLAYER_ALIVE:
			return checkAliveBounds(playerPosition);
		case PlayerBasics::PLAYER_GHOST:
			return checkGhostBounds(playerPosition);
		default:
			return playerPosition;
	}
}

FVector PlayerBasics::checkAliveBounds(FVector playerPosition)
{
	if (playerPosition.Z < PlayerBasics::MIN_MAP_POSITION_Z)
	{
		if (playerPosition.X > PlayerBasics::MAX_MAP_POSITION.X)
			playerPosition.X = PlayerBasics::MAX_MAP_POSITION.X - PlayerBasics::MAP_RESPAWN_POSITION_OFFSET;
		else if (playerPosition.X < -PlayerBasics::MAX_MAP_POSITION.X)
			playerPosition.X = -PlayerBasics::MAX_MAP_POSITION.X + PlayerBasics::MAP_RESPAWN_POSITION_OFFSET;

		if (playerPosition.Y > PlayerBasics::MAX_MAP_POSITION.Y)
			playerPosition.Y = PlayerBasics::MAX_MAP_POSITION.Y - PlayerBasics::MAP_RESPAWN_POSITION_OFFSET;
		else if (playerPosition.Y < -PlayerBasics::MAX_MAP_POSITION.Y)
			playerPosition.Y = -PlayerBasics::MAX_MAP_POSITION.Y + PlayerBasics::MAP_RESPAWN_POSITION_OFFSET;

		playerPosition.Z = PlayerBasics::PLAYER_SPAWN_POSITION_Z;

		this->makeGhost();

		this->m_fellOff = true;
	}

	return playerPosition;
}

FVector PlayerBasics::checkGhostBounds(FVector playerPosition)
{
	if (playerPosition.X > PlayerBasics::MAX_MAP_POSITION.X)
		playerPosition.X = PlayerBasics::MAX_MAP_POSITION.X;
	else if (playerPosition.X < -PlayerBasics::MAX_MAP_POSITION.X)
		playerPosition.X = -PlayerBasics::MAX_MAP_POSITION.X;

	if (playerPosition.Y > PlayerBasics::MAX_MAP_POSITION.Y)
		playerPosition.Y = PlayerBasics::MAX_MAP_POSITION.Y;
	else if (playerPosition.Y < -PlayerBasics::MAX_MAP_POSITION.Y)
		playerPosition.Y = -PlayerBasics::MAX_MAP_POSITION.Y;

	playerPosition.Z = PlayerBasics::PLAYER_SPAWN_POSITION_Z;

	return playerPosition;
}

FVector PlayerBasics::getInputDirection()
{
	return this->m_movement.getInputVector(this->m_cameraMovement.getCameraRotation().Yaw);
}

void PlayerBasics::makeAlive()
{
	this->m_playerState = PlayerBasics::PLAYER_ALIVE;

	//Model/Anims
	m_playerSkeletalMeshComponent->SetSkeletalMesh(m_playerModel);
}

void PlayerBasics::makeGrabbed()
{
	this->m_playerState = PlayerBasics::PLAYER_GRABBED;
}

void PlayerBasics::launchPlayer(FVector direction, FRotator rotation)
{
	if (direction.IsNearlyZero())
		this->m_movement.setMovementVelocity(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	else
	{
		FVector normalizedDirection = direction.GetUnsafeNormal();
		this->m_movement.setMovementVelocity(normalizedDirection, normalizedDirection);
		this->m_jumpZVelocity = getJumpVelocity(PlayerBasics::LAUNCH_HEIGHT);
		this->m_shouldJump = true;
	}

	this->m_movement.setRotation(rotation);

	this->m_currentMobility = PlayerBasics::LAUNCH_MOBILITY;
	this->updateAttributes();
}