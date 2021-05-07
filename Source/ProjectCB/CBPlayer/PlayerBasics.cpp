#include "PlayerBasics.h"
#include "Net/UnrealNetwork.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"

const FVector2D FPlayerBasics::MAX_MAP_POSITION(1475.0f, 975.0f);
const float FPlayerBasics::MAP_RESPAWN_POSITION_OFFSET = 50.0f;
const float FPlayerBasics::MIN_MAP_POSITION_Z = -200.0f;

const float FPlayerBasics::PLAYER_RADIUS = 25.0f;
const float FPlayerBasics::PLAYER_HEIGHT = 50.0f;

const float FPlayerBasics::PLAYER_SPAWN_POSITION_Z = 100.0f + FPlayerBasics::PLAYER_HEIGHT;

const float FPlayerBasics::PLAYER_BASE_GRAVITY = 4.0f;
const float FPlayerBasics::PLAYER_FAST_GRAVITY = 1.5f * FPlayerBasics::PLAYER_BASE_GRAVITY;

const float FPlayerBasics::PLAYER_START_WORLD_LOCATION_Z = 250.0f;
const float FPlayerBasics::WORLD_LOCATION_PROPORTION_Z = 0.75f;

const float FPlayerBasics::LAUNCH_SPEED = 100.0f;
const float FPlayerBasics::LAUNCH_MOBILITY = 0.25f;
const float FPlayerBasics::LAUNCH_HEIGHT = 50.0f;

const short FPlayerBasics::RESET_COLLISION_FRAMES = 3; // TODO remove?

FPlayerBasics::FPlayerBasics()
{
	this->m_playerState = PLAYER_ALIVE;
	this->m_dodgeState = DODGE_OFF;
	this->m_throwState = THROW_OFF;

	this->m_currentWorldLocationZ = FPlayerBasics::PLAYER_START_WORLD_LOCATION_Z;

	this->m_currentMobility = 1.0f;
	this->m_previousMobility = this->m_currentMobility;

	this->m_currentRadius = FPlayerBasics::PLAYER_RADIUS;

	this->m_currentHeight = FPlayerBasics::PLAYER_HEIGHT;
	this->m_previousHeight = this->m_currentHeight;

	this->m_jumpZVelocity = 0;

	this->m_shouldJump = false;
	this->m_fellOff = false;
}

float FPlayerBasics::getJumpVelocity(float height)
{
	return sqrt(5000 * (height + ((FPlayerBasics::PLAYER_HEIGHT - this->m_currentHeight) / 20.0f))
		* FPlayerBasics::PLAYER_BASE_GRAVITY);
}

float FPlayerBasics::getAnimationPoint(float x)
{
	return x; // TODO adjust to sync with procedural animations (add additional parameters where needed)
}

void FPlayerBasics::updateAttributes()
{
	this->m_previousMobility = this->m_currentMobility;
	this->m_previousHeight = this->m_currentHeight;
}

void FPlayerBasics::updateGroundState(bool grounded)
{
	this->m_grounded = grounded;

	if(this->m_grounded)
		this->m_currentMobility = 1.0f;

	this->m_movement.isGrounded(this->m_grounded,
		this->m_playerState == FPlayerBasics::PlayerState::PLAYER_GHOST);
}

bool FPlayerBasics::isGrounded()
{
	return this->m_grounded;
}

FPlayerBasics::PlayerState FPlayerBasics::getPlayerState()
{
	return this->m_playerState;
}

void FPlayerBasics::makeGhost()
{
	this->m_playerState = FPlayerBasics::PLAYER_GHOST;
	this->m_dodgeState = FPlayerBasics::DODGE_OFF;
	this->m_throwState = FPlayerBasics::THROW_OFF;

	this->m_currentMobility = 1.0f;
	this->m_currentHeight = FPlayerBasics::PLAYER_HEIGHT;

	// DROP GRABBED ITEM

	//Model/Anims
	//m_playerSkeletalMeshComponent->SetSkeletalMesh(m_ghostModel);

	updateAttributes();

	// TODO update other attributes for ghost such as GHOST_SPEED
}
//Send this position to server
FVector FPlayerBasics::checkPlayerBounds(FVector playerPosition)
{
	switch (getPlayerState())
	{
		case FPlayerBasics::PLAYER_ALIVE:
			return checkAliveBounds(playerPosition);
		case FPlayerBasics::PLAYER_GHOST:
			return checkGhostBounds(playerPosition);
		default:
			return playerPosition;
	}
}

FVector FPlayerBasics::checkAliveBounds(FVector playerPosition)
{
	if (playerPosition.Z < FPlayerBasics::MIN_MAP_POSITION_Z)
	{
		if (playerPosition.X > FPlayerBasics::MAX_MAP_POSITION.X)
			playerPosition.X = FPlayerBasics::MAX_MAP_POSITION.X - FPlayerBasics::MAP_RESPAWN_POSITION_OFFSET;
		else if (playerPosition.X < -FPlayerBasics::MAX_MAP_POSITION.X)
			playerPosition.X = -FPlayerBasics::MAX_MAP_POSITION.X + FPlayerBasics::MAP_RESPAWN_POSITION_OFFSET;

		if (playerPosition.Y > FPlayerBasics::MAX_MAP_POSITION.Y)
			playerPosition.Y = FPlayerBasics::MAX_MAP_POSITION.Y - FPlayerBasics::MAP_RESPAWN_POSITION_OFFSET;
		else if (playerPosition.Y < -FPlayerBasics::MAX_MAP_POSITION.Y)
			playerPosition.Y = -FPlayerBasics::MAX_MAP_POSITION.Y + FPlayerBasics::MAP_RESPAWN_POSITION_OFFSET;

		playerPosition.Z = FPlayerBasics::PLAYER_SPAWN_POSITION_Z;

		
		ACB_PlayerCharacter* player = Cast<ACB_PlayerCharacter>(this->m_playerRef);
		if (player != nullptr)
		{
			if (player->bIsMultiplayer)
			{
				player->MakePlayerIntoGhost();
			}
			else
			{//Make player alive
				this->makeGhost();
			}
		}

		this->m_fellOff = true;
	}

	return playerPosition;
}

FVector FPlayerBasics::checkGhostBounds(FVector playerPosition)
{
	if (playerPosition.X > FPlayerBasics::MAX_MAP_POSITION.X)
		playerPosition.X = FPlayerBasics::MAX_MAP_POSITION.X;
	else if (playerPosition.X < -FPlayerBasics::MAX_MAP_POSITION.X)
		playerPosition.X = -FPlayerBasics::MAX_MAP_POSITION.X;

	if (playerPosition.Y > FPlayerBasics::MAX_MAP_POSITION.Y)
		playerPosition.Y = FPlayerBasics::MAX_MAP_POSITION.Y;
	else if (playerPosition.Y < -FPlayerBasics::MAX_MAP_POSITION.Y)
		playerPosition.Y = -FPlayerBasics::MAX_MAP_POSITION.Y;

	playerPosition.Z = FPlayerBasics::PLAYER_SPAWN_POSITION_Z;

	return playerPosition;
}

FVector FPlayerBasics::getInputDirection()
{
	return this->m_movement.getInputVector(this->m_cameraMovement.getCameraRotation().Yaw);
}

void FPlayerBasics::makeAlive()
{
	this->m_playerState = FPlayerBasics::PLAYER_ALIVE;

	//Model/Anims
	m_playerSkeletalMeshComponent->SetSkeletalMesh(m_playerModel);
}

void FPlayerBasics::makeGrabbed()
{
	this->m_playerState = FPlayerBasics::PLAYER_GRABBED;
}

void FPlayerBasics::launchPlayer(FVector direction, FRotator rotation)
{
	if (direction.IsNearlyZero())
		this->m_movement.setMovementVelocity(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	else
	{
		FVector normalizedDirection = direction.GetUnsafeNormal();
		this->m_movement.setMovementVelocity(normalizedDirection, normalizedDirection);
		this->m_jumpZVelocity = getJumpVelocity(FPlayerBasics::LAUNCH_HEIGHT);
		this->m_shouldJump = true;
	}

	this->m_movement.setRotation(rotation);

	this->m_currentMobility = FPlayerBasics::LAUNCH_MOBILITY;
	this->updateAttributes();
}