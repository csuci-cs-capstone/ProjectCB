// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerCharacter.h"
#include "ProjectCB/CBObjects/CB_DodgeballProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include <Animation/AnimSingleNodeInstance.h>
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectCB/CBPlayer/CB_PlayerState.h"
#include "ProjectCB/CBPlayer/CB_PlayerController.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"


#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s"), text));

// Sets default values
ACB_PlayerCharacter::ACB_PlayerCharacter()
{
	this->SetReplicates(true);
	this->SetReplicateMovement(true);
	this->bAlwaysRelevant = true;
	this->SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->m_basics.m_playerRef = this;

	//Move States Classes
	

	GetCapsuleComponent()->InitCapsuleSize(25.0f, 50.0f); // TODO create default size

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	this->skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	this->skeletalMesh->SetupAttachment(this->RootComponent);

	//LOAD in from uassets here
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMeshAsset(TEXT("SkeletalMesh'/Game/PlayerBP/PrototypeSkeletalModel/Character1Asset.Character1Asset'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ghostMeshAsset(TEXT("SkeletalMesh'/Game/PlayerBP/PrototypeGhostModel/GhostSkeletal/GhostPrototypeModel.GhostPrototypeModel_GhostPrototypeModel'"));
	
	this->m_basics.m_playerModel = playerMeshAsset.Object;
	this->m_basics.m_ghostModel = ghostMeshAsset.Object;
	this->m_PlayerGhostModel = ghostMeshAsset.Object;
	this->m_basics.m_playerSkeletalMeshComponent = skeletalMesh;
	

	skeletalMesh->SetSkeletalMesh(playerMeshAsset.Object);

	//Customize the character movement component here!
	GetCharacterMovement()->MaxWalkSpeed = Movement::PLAYER_GROUND_SPEED;
	GetCharacterMovement()->GravityScale = FPlayerBasics::PLAYER_BASE_GRAVITY;
	
	if (this->bIsMultiplayer)
	{
		GetCharacterMovement()->JumpZVelocity = 1375.0f;
		GetCharacterMovement()->AirControl = 1.0f;
	}
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Character will look in the direction of movement
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//Character will look in the direction of the axis controlling the camera
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	this->GetCharacterMovement()->SetIsReplicated(true);

	this->cameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArm");
	this->cameraArm->SetupAttachment(this->RootComponent);
	this->cameraArm->TargetArmLength = 500.0f; // TODO add to CameraMovement class
	
	this->camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	this->camera->SetupAttachment(this->cameraArm);
	
	this->cameraArm->bUsePawnControlRotation = false;
	this->camera->bUsePawnControlRotation = false;

	//Set Locations and Rotations after attachments have been set
	this->cameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	this->cameraArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	this->camera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	/*      *\
	   Grab
	\*      */

	this->grabRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GrabRoot"));
	this->grabRoot->SetRelativeLocation(FVector(2 * FPlayerBasics::PLAYER_RADIUS, 0.0f, FPlayerBasics::PLAYER_HEIGHT));
	this->grabRoot->SetupAttachment(skeletalMesh);

	this->grabBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBox"));
	this->grabBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->grabBox->SetGenerateOverlapEvents(true);
	this->grabBox->OnComponentBeginOverlap.AddDynamic(this, &ACB_PlayerCharacter::OnEnterGrabBox);
	this->grabBox->OnComponentEndOverlap.AddDynamic(this, &ACB_PlayerCharacter::OnLeaveGrabBox);
	this->grabBox->SetupAttachment(this->grabRoot);
}


// Network Replication for playerstate use for player specific values
void ACB_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	APlayerState* OwningPlayerState = this->GetPlayerState();

	if (OwningPlayerState != nullptr)
	{
		ACB_PlayerState* CBOwningPlayerState = Cast<ACB_PlayerState>(OwningPlayerState);
	}
}

// Called when the game starts or when spawned
void ACB_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	this->m_throw = NewObject<UThrow>();
	this->m_throw->setPlayerBasics(this->m_basics);

	this->m_resetCollisionFrame = FPlayerBasics::RESET_COLLISION_FRAMES + 1;

	this->m_basics.m_gameWorldRef = GetWorld();
	this->m_basics.dodgeballClassRef = DodgeballProjectileClass;
	//SetPlayerStartRotation();
	//this->m_basics.m_movement.setStartRotation(this->cameraArm->GetComponentRotation());
	//this->cameraArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 180.0f));
	//this->m_basics.m_movement.setStartRotation(this->GetActorRotation());

	if (HasAuthority() == false)
	{
		SetPlayerMaterialColor();
	}

	if (this->bIsMultiplayer == false)
	{
		ACB_PlayerController* CBController = Cast<ACB_PlayerController>(GetController());

		if (CBController != nullptr)
		{
			CBController->SetPlayerControlEnabled(true);
		}
	}

}

void ACB_PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACB_PlayerCharacter, m_basics);

	DOREPLIFETIME(ACB_PlayerCharacter, DynamicMaterial);

	DOREPLIFETIME(ACB_PlayerCharacter, m_throw);

	DOREPLIFETIME(ACB_PlayerCharacter, grabRoot);

	DOREPLIFETIME(ACB_PlayerCharacter, bIsOnGroundAnimate);

	DOREPLIFETIME(ACB_PlayerCharacter, StartTransform);

	DOREPLIFETIME(ACB_PlayerCharacter, bIsMultiplayer);

	DOREPLIFETIME(ACB_PlayerCharacter, bIsGhost);
}

// Called every frame
void ACB_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	playerUpdate(DeltaTime);

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	adjustGravity(characterMovement);

	this->m_dodge.update(DeltaTime);
	if (this->m_throw != nullptr)
	{
		this->m_throw->update(this->GetActorLocation(), this->GetActorRotation(), DeltaTime);
	}
	

// START RADIUS UPDATE

	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	
	capsuleComponent->SetCapsuleSize(this->m_basics.m_currentRadius, this->m_basics.m_currentHeight);
	this->skeletalMesh->SetRelativeLocation(FVector(FPlayerBasics::PLAYER_RADIUS - this->m_basics.m_currentRadius, 0.0f,
		-FPlayerBasics::PLAYER_HEIGHT));
	
	

// END RADIUS UPDATE

	if (this->bIsMultiplayer == false)
	{
		cameraUpdate();
	}
	
	this->m_basics.m_movement.resetInputVelocity();

}

void ACB_PlayerCharacter::playerUpdate(float deltaTime)
{
	if (Controller != nullptr)
	{
		//this->SetActorLocation(this->m_basics.checkPlayerBounds(this->GetActorLocation()));
		this->m_basics.m_movement.setInputRotation(Controller->GetControlRotation().Yaw);

		UCharacterMovementComponent* characterMovement = GetCharacterMovement();

		this->m_basics.updateGroundState(characterMovement->IsMovingOnGround());

		if (this->m_basics.m_shouldJump) // TODO set jump movement to 1 to jump
		{
			characterMovement->JumpZVelocity = this->m_basics.m_jumpZVelocity;

			Jump();

			this->m_basics.m_shouldJump = false;
		}

		if (this->m_basics.m_fellOff)
		{
			this->m_throw->drop();

			this->m_basics.m_fellOff = false;
		}

		this->m_basics.m_movement.updateVelocity(this->m_basics.m_currentMobility);

		if (this->bIsMultiplayer == false)
		{
			GetCharacterMovement()->Velocity = this->m_basics.m_movement.getMovementVelocity(GetCharacterMovement()->Velocity.Z);
		}

		CheckIfPlayerIsAlive();

		if (this->m_resetCollisionFrame >= 0)
		{
			if (this->m_resetCollisionFrame < FPlayerBasics::RESET_COLLISION_FRAMES)
				this->m_resetCollisionFrame++;
			else if (this->m_resetCollisionFrame == FPlayerBasics::RESET_COLLISION_FRAMES)
			{
				this->SetActorEnableCollision(true);
				this->m_resetCollisionFrame++;
			}
		}
	}
}

void ACB_PlayerCharacter::cameraUpdate()
{
	FVector currentLocation = this->cameraArm->GetComponentLocation();

	const FRotator& playerRotation = this->m_basics.m_movement.getPlayerRotation();

	//Send to server either through player 
	this->GetCapsuleComponent()->SetRelativeRotation(playerRotation);
	this->cameraArm->SetRelativeRotation(this->m_basics.m_cameraMovement.getCameraRotation() - playerRotation);

	this->m_basics.m_currentWorldLocationZ = ((1 - FPlayerBasics::WORLD_LOCATION_PROPORTION_Z) * this->GetActorLocation().Z)
		+ (FPlayerBasics::WORLD_LOCATION_PROPORTION_Z * FPlayerBasics::PLAYER_START_WORLD_LOCATION_Z);
	//Send to server
	this->cameraArm->SetWorldLocation(FVector(currentLocation.X, currentLocation.Y, this->m_basics.m_currentWorldLocationZ));

	
	this->grabBox->SetRelativeRotation(playerRotation);
	//Quick way to make sure grabBox follows model orientation
	this->grabBox->SetRelativeRotation(this->skeletalMesh->GetRelativeRotation());
}

void ACB_PlayerCharacter::adjustGravity(UCharacterMovementComponent* characterMovement)
{
	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = FPlayerBasics::PLAYER_FAST_GRAVITY;
	else
		characterMovement->GravityScale = FPlayerBasics::PLAYER_BASE_GRAVITY;
}

// Called to bind functionality to input
void ACB_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACB_PlayerCharacter::MoveVertical(float amount)
{
	this->m_basics.m_currentScaleAmountX = amount;
	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw + this->m_basics.m_cameraMovement.getCameraRotation().Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		this->m_basics.m_movement.addInputVector(movementDirection * amount);
		if (bIsMultiplayer)
		{
			this->AddMovementInput(movementDirection, amount);
		}
		
	}
}

void ACB_PlayerCharacter::MoveHorizontal(float amount)
{
	this->m_basics.m_currentScaleAmountY = amount;
	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw + this->m_basics.m_cameraMovement.getCameraRotation().Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		this->m_basics.m_movement.addInputVector(movementDirection * amount);
		if (this->bIsMultiplayer)
		{
			this->AddMovementInput(movementDirection, amount);
		}
		
	}
}

void ACB_PlayerCharacter::updateCameraSensitivity(float cameraSensitivity)
{
	this->m_basics.m_cameraMovement.setCameraSensitivity(cameraSensitivity);
}

void ACB_PlayerCharacter::RotateCamera(float amount)
{
	if (this->bIsMultiplayer)
	{
		AddControllerYawInput(amount * 75.0f * GetWorld()->GetDeltaSeconds());
	}
	else 
	{
		this->m_basics.m_cameraMovement.updateCamera(this->m_basics.m_movement, amount);
	}
}

void ACB_PlayerCharacter::JumpAction()
{
	this->m_dodge.onPress();
}

void ACB_PlayerCharacter::StopJumpAction()
{
	if (this->bIsMultiplayer)
	{
		Jump();
	}
	this->m_dodge.onRelease();
}

void ACB_PlayerCharacter::ShootAction() // TODO create a Dodgeball Generator
{
	this->m_throw->onPress();
}

void ACB_PlayerCharacter::StopShootAction()
{
	this->m_throw->onRelease(this->GetActorRotation());
}

void ACB_PlayerCharacter::AliveAction()
{
	//this->m_basics.makeAlive();
}

// Grab

void ACB_PlayerCharacter::OnEnterGrabBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult)
{
	if (this == otherActor)
		return;	

	this->m_throw->m_grabbableList.add(Cast<IGrabbable>(otherActor));
}

void ACB_PlayerCharacter::OnLeaveGrabBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex)
{
	if (this == otherActor)
		return;

	this->m_throw->m_grabbableList.remove(Cast<IGrabbable>(otherActor));
}

//Networked Moves//
bool ACB_PlayerCharacter::SendLocalClientRotationToServer_Validate()
{
	return true;
}

void ACB_PlayerCharacter::SendLocalClientRotationToServer_Implementation()
{
	//Todo send rotation to server
	bool test = true;
}

//NETWORK RPCs//
void ACB_PlayerCharacter::UpdateVelocity_Implementation(FVector newVelocityVector) 
{
	FVector CurrentLocation = this->GetActorLocation();

	CurrentLocation = CurrentLocation + newVelocityVector;

	//GetCharacterMovement()->Velocity = newVelocityVector;

	//SetActorLocation(CurrentLocation);

	this->OnRep_ReplicateMovement();
}

void ACB_PlayerCharacter::KnockBackPlayer_Implementation(FVector HitNormal)
{
	FVector KnockBackVector = -HitNormal;
	FVector LaunchForce = KnockBackVector * 1000;
	LaunchForce.Z = 1000;
	this->LaunchCharacter(LaunchForce, true, true);
}


void ACB_PlayerCharacter::CheckIfPlayerIsAlive_Implementation()
{
	this->m_basics.checkPlayerBounds(this->GetActorLocation());
}

void ACB_PlayerCharacter::MakePlayerIntoGhost_Implementation()
{
	this->skeletalMesh->SetSkeletalMesh(this->m_basics.m_ghostModel);
	this->bIsGhost = true;

	ACB_PlayerController* CBController = Cast<ACB_PlayerController>(this->GetController());

	if (CBController != nullptr)
	{
		FVector NewSpawnLocation = FVector(CBController->PlayerStartLocation.X, CBController->PlayerStartLocation.Y, 150.0f);
		this->SetActorLocation(NewSpawnLocation);
		this->SetActorRotation(CBController->PlayerStartRotation.Rotation());
	}

	this->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	this->GetCharacterMovement()->StopActiveMovement();
	this->GetCharacterMovement()->StopMovementImmediately();

	//this->SetActorLocation(StartTransform.GetLocation());
	//this->SetActorLocation(StartTransform.GetRotation().Rotator().Vector());
}

void ACB_PlayerCharacter::MakePlayerAlive_Implementation() 
{
	this->skeletalMesh->SetSkeletalMesh(this->m_basics.m_playerModel);

	ACB_PlayerController* CBController = Cast<ACB_PlayerController>(this->GetController());

	if (CBController != nullptr)
	{
		this->SetActorLocation(CBController->PlayerStartLocation);
		this->SetActorRotation(CBController->PlayerStartRotation.Rotation());
	}

	this->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ACB_PlayerCharacter::SetPlayerStartPosition_Implementation(FVector incomingPosition, FRotator incomingRotation)
{
	StartTransform.SetLocation(incomingPosition);
	StartTransform.SetRotation(incomingRotation.Quaternion());
}

void ACB_PlayerCharacter::SetPlayerMaterialColor_Implementation()
{
	auto Material = skeletalMesh->GetMaterial(0);
	auto HeadMaterial = skeletalMesh->GetMaterial(3);

	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, NULL);
	skeletalMesh->SetMaterial(0, DynamicMaterial);

	DynamicHeadMaterial = UMaterialInstanceDynamic::Create(HeadMaterial, NULL);
	skeletalMesh->SetMaterial(3, DynamicHeadMaterial);

	float TeamColor = 0.0f;
	ACB_PlayerState* CBPlayerState = Cast<ACB_PlayerState>(GetPlayerState());
	if (CBPlayerState != nullptr)
	{
		if (CBPlayerState->Team == "yellow")
		{
			TeamColor = 1.0f;
		}
	}

	DynamicMaterial->SetScalarParameterValue(TEXT("Blend"), TeamColor);
	DynamicHeadMaterial->SetScalarParameterValue(TEXT("Blend"), TeamColor);
}

bool ACB_PlayerCharacter::SetPlayerMaterialColor_Validate()
{
	return true;
}

void ACB_PlayerCharacter::SetPlayerStartRotation_Implementation()
{
	ACB_PlayerState* CBPlayerState = Cast<ACB_PlayerState>(GetPlayerState());

	if (CBPlayerState != nullptr)
	{
		if (CBPlayerState->Team == "yellow")
		{
			this->SetActorRotation(FRotator(0.0f, 0.0f, 180.0f));
			this->cameraArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 180.0f));
		}
		else
		{
			this->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
			this->cameraArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
		}
	}
}

bool ACB_PlayerCharacter::SetPlayerStartRotation_Validate()
{
	return true;
}

float ACB_PlayerCharacter::getRadius()
{
	return FPlayerBasics::PLAYER_RADIUS;
}

bool ACB_PlayerCharacter::isGrabbable()
{
	return this->m_basics.getPlayerState() == FPlayerBasics::PLAYER_ALIVE;
}

void ACB_PlayerCharacter::makeGrabbed()
{
	this->grabBox->SetGenerateOverlapEvents(false);
	this->m_basics.makeGrabbed();
}


//NETWORKED TRHOW CLASS FUNCTIONS
void ACB_PlayerCharacter::LaunchBall_Implementation()
{
	FActorSpawnParameters spawnParameters;

	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	spawnParameters.bNoFail = true;
	spawnParameters.Owner = NULL;
	spawnParameters.Instigator = this;
	spawnParameters.bDeferConstruction = false;

	//FTransform spawnTransform = this->m_throw->m_grabTransform;
	FTransform spawnTransform;

	FVector spawnLocation = GetActorForwardVector() * 125.0f
				+ FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);

	spawnTransform.SetLocation(spawnLocation);
	spawnTransform.SetScale3D(FVector(0.5f));

	auto dodgeball = GetWorld()->SpawnActor<ACB_DodgeballProjectile>(DodgeballProjectileClass,
		spawnTransform, spawnParameters);

	this->m_basics.m_throwing = true;

	dodgeball->m_playerRef = this;
	this->ignoreCollisionsOnThrownObject(dodgeball);

	//dodgeball->launchRelease(this->m_playerBasics->m_movement.getPlayerRotation().RotateVector(FVector(1.0f, 0.0f, 0.0f)), playerRotation);
	if (this->bIsMultiplayer)
	{
		if (this->bIsGhost)
		{

		}
		else
		{
			dodgeball->launchRelease(this->GetActorForwardVector(), GetActorRotation());
		}
	}
	else
	{
		dodgeball->launchRelease(this->GetActorForwardVector(), GetActorRotation());
	}
	

	dodgeball->SetReplicates(true);
	dodgeball->SetReplicateMovement(true);
}

void ACB_PlayerCharacter::RemoveBall_Implementation(ACB_DodgeballProjectile* currentBall)
{
	currentBall->Destroy();
}

void ACB_PlayerCharacter::UpdateGrabbedObjectPosition_Implementation(UObject* currentGrabbedObject)
{
	FVector currentLocation = GetActorLocation();
	FRotator currentRotation = GetActorRotation();

	AActor* GrabbedActor = Cast<AActor>(currentGrabbedObject);
	IGrabbableObject* GrabbedObjectInfo = Cast<IGrabbableObject>(currentGrabbedObject);

	if (GrabbedActor != nullptr)
	{
		//currentGrabbedObject->setGrabbed(currentLocation + currentRotation.RotateVector(FVector(UThrow::GRAB_OFFSET
		//	+ currentGrabbedObject->getRadius(), 0.0f, 0.0f)), currentRotation);

		FVector rotatedVector = currentRotation.RotateVector(FVector(UThrow::GRAB_OFFSET + GrabbedObjectInfo->getRadius(), 0.0f, 0.0f));
		//GrabbedObjectInfo->makeGrabbed();
		GrabbedObjectInfo->setGrabbed(currentLocation + rotatedVector, currentRotation);
		//TODO switch collision channel at runtime for grabbed
	}
}



void ACB_PlayerCharacter::launchRelease(FVector direction, FRotator rotation)
{
	this->m_resetCollisionFrame = 0;
	this->m_basics.makeAlive();
	this->m_basics.launchPlayer(direction, rotation);
}

void ACB_PlayerCharacter::setGrabbed(FVector position, FRotator rotation)
{
	this->m_resetCollisionFrame = -1;
	this->SetActorEnableCollision(false);
	this->SetActorLocation(position);
	this->GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);
	this->m_basics.m_movement.setRotation(rotation);
}

bool ACB_PlayerCharacter::hasGrabbableObject()
{
	return this->isGrabbable();
}

IGrabbableObject* ACB_PlayerCharacter::getGrabbableObject()
{
	return this;
}

unsigned char ACB_PlayerCharacter::getGrabPriority()
{
	return UGrabbable::PLAYER_PRIORITY;
}


//ANIM HELPERS
bool ACB_PlayerCharacter::onGround()
{
	this->bIsOnGroundAnimate = this->GetMovementComponent()->CanEverMoveOnGround();
	return this->bIsOnGroundAnimate;
}

bool ACB_PlayerCharacter::onDuck()
{
	return this->m_basics.m_ducking;
}

bool ACB_PlayerCharacter::onThrowing()
{
	bool objectThrown = this->m_basics.m_throwing;
	//this->m_basics.m_throwing = false;
	return objectThrown;
}

void ACB_PlayerCharacter::resetThrowing()
{
	this->m_basics.m_throwing = false;
}

float ACB_PlayerCharacter::groundMoveSpeed()
{
	FVector groundVelocity = this->GetVelocity();
	groundVelocity.Z = 0;
	float currentSpeed = groundVelocity.Size();
	return currentSpeed;
}

bool ACB_PlayerCharacter::onCatch()
{
	//bool objectCatched = (this->m_throw->m_grabbableList.length() == 0) ? false : true;
	bool objectCatched = false;
	if (this->m_throw != nullptr && this->m_throw->m_grabbableList.isValidList())
	{
		if (this->m_throw->m_grabbableList.length() == 0)
		{
			objectCatched = false;
		}
		else
		{
			objectCatched = true;
		}
	}
	return objectCatched;
}

void ACB_PlayerCharacter::ignoreCollisionsOnThrownObject(AActor* spawnedActor)
{
	this->MoveIgnoreActorAdd(spawnedActor);
}


