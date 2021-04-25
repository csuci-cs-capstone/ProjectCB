#include "Throw.h"
#include "ProjectCB/CBObjects/CB_DodgeballProjectile.h"
#include "ProjectCB/CBPlayer/CB_PlayerCharacter.h"

const FVector UThrow::THROW_DIRECTION = FVector(1, 0, 0.025).GetUnsafeNormal();
const float UThrow::GRAB_OFFSET = 0.0f;

UThrow::UThrow()
{
	//Default Constructor needed for UObject Inheritance
}

void UThrow::setPlayerBasics(FPlayerBasics& playerBasics)
{
	this->m_playerBasics = &playerBasics;
}

void UThrow::onPress_Implementation()
{
	if (!this->m_playerBasics->m_throwState)
	{
		if (this->m_grabbedObject)
			this->m_playerBasics->m_throwState = FPlayerBasics::THROW_STARTUP;
		else
			this->m_playerBasics->m_throwState = FPlayerBasics::CATCH_STARTUP;
	}
}

bool UThrow::onPress_Validate()
{
	return true;
}

void UThrow::onRelease_Implementation(FRotator playerRotation)
{
	if (this->m_playerBasics->m_throwState == FPlayerBasics::THROW_AIM) // TODO should buffer for startup
	{
		this->m_playerBasics->m_currentRadius = FPlayerBasics::PLAYER_RADIUS;

		switch (this->m_playerBasics->getPlayerState())
		{
			case FPlayerBasics::PLAYER_ALIVE:
				
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
			case FPlayerBasics::PLAYER_GRABBED:
				this->m_grabbedObject->launchRelease( // TODO should probably ensure it can't not exist in throw state
					this->m_playerBasics->m_movement.getPlayerRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)),
					playerRotation);
				break;
			default:
				this->m_grabbedObject->launchRelease(FVector(0.0f, 0.0f, 0.0f), playerRotation);
				break;
		}

		this->m_grabbedObject = nullptr;

		this->m_playerBasics->m_throwState = FPlayerBasics::THROW_COOLDOWN;
	}

	else if (this->m_playerBasics->m_throwState == FPlayerBasics::CATCH_AIM) // TODO should buffer for startup
		this->m_playerBasics->m_throwState = FPlayerBasics::CATCH_COOLDOWN;
}

bool UThrow::onRelease_Validate(FRotator playerRotation) 
{
	return true;
}

void UThrow::update_Implementation(FVector playerPosition, FRotator playerRotation, float deltaTime)
{
	if (this->m_playerBasics == nullptr)
		return;

	switch (this->m_playerBasics->m_throwState)
	{
	case FPlayerBasics::CATCH_STARTUP:
		this->catchStartUpdate(deltaTime);
		break;
	case FPlayerBasics::THROW_STARTUP:
		this->throwStartUpdate(deltaTime);
		break;
	case FPlayerBasics::CATCH_AIM:
		this->catchAimUpdate(deltaTime);
		break;
	case FPlayerBasics::THROW_AIM:
		this->throwAimUpdate(deltaTime);
		break;
	case FPlayerBasics::CATCH_COOLDOWN:
		this->catchCooldownUpdate(deltaTime);
		break;
	case FPlayerBasics::THROW_COOLDOWN:
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

	//if (this->m_grabbedObject != nullptr)
	//	this->m_grabbedObject->setGrabbed(playerPosition + playerRotation.RotateVector(FVector(UThrow::GRAB_OFFSET
	//		+ this->m_grabbedObject->getRadius(), 0.0f, 0.0f)), playerRotation);
	
	if (this->m_grabbedObject != nullptr)
	{
		this->updateCurrentlyGrabbed(this->m_grabbedObject);
	}
}

bool UThrow::update_Validate(FVector playerPosition, FRotator playerRotation, float deltaTime) 
{
	return true;
}

void UThrow::drop_Implementation()
{
	if (this->m_grabbedObject != nullptr)
	{
		this->m_grabbedObject->launchRelease(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
		this->m_grabbedObject = nullptr;
	}
}

bool UThrow::drop_Validate() 
{
	return true;
}

void UThrow::removeBall_Implementation()
{
	ACB_DodgeballProjectile* currentBall = Cast<ACB_DodgeballProjectile>(m_grabbedObject->_getUObject());
	this->m_grabTransform = currentBall->GetActorTransform();

	//currentBall->SetOwner(this->m_playerBasics->m_playerRef);

	//currentBall->Destroy();
	auto playerBody = Cast<ACB_PlayerCharacter>(this->m_playerBasics->m_playerRef);
	playerBody->RemoveBall(currentBall);
}

bool UThrow::removeBall_Validate()
{
	return true;
}

void UThrow::launchBall_Implementation(FRotator playerRotation)
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->m_playerBasics->m_playerRef);
	playerBody->LaunchBall();
}

void UThrow::updateCurrentlyGrabbed(IGrabbableObject* currentGrabbedObject)
{
	auto playerBody = Cast<ACB_PlayerCharacter>(this->m_playerBasics->m_playerRef);
	playerBody->UpdateGrabbedObjectPosition(currentGrabbedObject->_getUObject());
	//this->m_playerBasics->m_playerRef->UpdateGrabbedObjectPosition(currentGrabbedObject->_getUObject());
}

bool UThrow::IsNameStableForNetworking() const
{
	return true;
}
