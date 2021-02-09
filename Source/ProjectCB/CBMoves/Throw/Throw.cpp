#include "Throw.h"
#include "ProjectCB/CBObjects/CB_DodgeballProjectile.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"

const FVector Throw::THROW_DIRECTION = FVector(1, 0, 0.025).GetUnsafeNormal();
const float Throw::GRAB_OFFSET = 0.0f;

Throw::Throw(PlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
}

void Throw::onPress()
{
	if (!this->m_playerBasics->m_throwState)
	{
		if (this->m_grabbedObject)
			this->m_playerBasics->m_throwState = PlayerBasics::THROW_STARTUP;
		else
			this->m_playerBasics->m_throwState = PlayerBasics::CATCH_STARTUP;
	}
}

void Throw::onRelease(FRotator playerRotation)
{
	if (this->m_playerBasics->m_throwState == PlayerBasics::THROW_AIM) // TODO should buffer for startup
	{
		this->m_playerBasics->m_currentRadius = PlayerBasics::PLAYER_RADIUS;

		switch (this->m_playerBasics->getPlayerState())
		{
			case PlayerBasics::PLAYER_ALIVE:
				
				/*this->m_grabbedObject->launchRelease( // TODO should probably ensure it can't not exist in throw state
					this->m_playerBasics->m_movement.getPlayerRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)),
					playerRotation);*/
				if (this->m_grabbedObject->_getUObject()->IsA(ACB_DodgeballProjectile::StaticClass())) 
				{
					removeBall();
					launchBall(playerRotation);
				}
				else
				{
					this->m_grabbedObject->launchRelease( // TODO should probably ensure it can't not exist in throw state
						this->m_playerBasics->m_movement.getPlayerRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)),
						playerRotation);
				}
				
				break;
			case PlayerBasics::PLAYER_GRABBED:
				this->m_grabbedObject->launchRelease( // TODO should probably ensure it can't not exist in throw state
					this->m_playerBasics->m_movement.getPlayerRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)),
					playerRotation);
				break;
			default:
				this->m_grabbedObject->launchRelease(FVector(0.0f, 0.0f, 0.0f), playerRotation);
				break;
		}

		this->m_grabbedObject = nullptr;

		this->m_playerBasics->m_throwState = PlayerBasics::THROW_COOLDOWN;
	}

	else if (this->m_playerBasics->m_throwState == PlayerBasics::CATCH_AIM) // TODO should buffer for startup
		this->m_playerBasics->m_throwState = PlayerBasics::CATCH_COOLDOWN;
}

void Throw::update(FVector playerPosition, FRotator playerRotation, float deltaTime)
{
	switch (this->m_playerBasics->m_throwState)
	{
	case PlayerBasics::CATCH_STARTUP:
		this->catchStartUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_STARTUP:
		this->throwStartUpdate(deltaTime);
		break;
	case PlayerBasics::CATCH_AIM:
		this->catchAimUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_AIM:
		this->throwAimUpdate(deltaTime);
		break;
	case PlayerBasics::CATCH_COOLDOWN:
		this->catchCooldownUpdate(deltaTime);
		break;
	case PlayerBasics::THROW_COOLDOWN:
		this->throwCooldownUpdate(deltaTime);
		break;
	default:
		//if (this->m_buffer) // TODO create buffer?
		//{
		//	this->startDuck();
		//	this->m_buffer = false;
		//}
		break;
	}

	if (this->m_grabbedObject)
		this->m_grabbedObject->setGrabbed(playerPosition + playerRotation.RotateVector(FVector(Throw::GRAB_OFFSET
			+ this->m_grabbedObject->getRadius(), 0.0f, 0.0f)), playerRotation);

}

void Throw::drop()
{
	if (this->m_grabbedObject)
	{
		this->m_grabbedObject->launchRelease(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		this->m_grabbedObject = nullptr;
	}
}

void Throw::removeBall()
{
	ACB_DodgeballProjectile* currentBall = Cast<ACB_DodgeballProjectile>(m_grabbedObject->_getUObject());
	this->m_grabTransform = currentBall->GetActorTransform();

	currentBall->Destroy();
}

void Throw::launchBall(FRotator playerRotation)
{
	FActorSpawnParameters spawnParameters;

	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	spawnParameters.bNoFail = true;
	spawnParameters.Owner = NULL;
	spawnParameters.Instigator = NULL;

	FTransform spawnTransform = this->m_grabTransform;
	
	auto dodgeball = this->m_playerBasics->m_gameWorldRef->SpawnActor<ACB_DodgeballProjectile>(this->m_playerBasics->dodgeballClassRef, 
		spawnTransform, spawnParameters);

	dodgeball->m_playerRef = this->m_playerBasics->m_playerRef;
	auto playerBody = Cast<ACB_PlayerCharacter>(this->m_playerBasics->m_playerRef);
	playerBody->ignoreCollisionsOnThrownObject(dodgeball);
	dodgeball->launchRelease(this->m_playerBasics->m_movement.getPlayerRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)), playerRotation);

}
