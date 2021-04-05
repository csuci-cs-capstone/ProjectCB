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
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"


#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s"), text));

// Sets default values
ACB_PlayerCharacter::ACB_PlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->m_basics.m_playerRef = this;

	GetCapsuleComponent()->InitCapsuleSize(25.0f, 50.0f); // TODO create default size

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	this->skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	this->skeletalMesh->SetupAttachment(this->RootComponent);

	//LOAD in from uassets here
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMeshAsset(TEXT("SkeletalMesh'/Game/PlayerBP/PrototypeSkeletalModel/Character1Asset.Character1Asset'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ghostMeshAsset(TEXT("SkeletalMesh'/Game/PlayerBP/PrototypeGhostModel/GhostSkeletal/GhostPrototypeModel.GhostPrototypeModel_GhostPrototypeModel'"));
	
	this->m_basics.m_playerModel = playerMeshAsset.Object;
	this->m_basics.m_ghostModel = ghostMeshAsset.Object;
	this->m_basics.m_playerSkeletalMeshComponent = skeletalMesh;
	

	skeletalMesh->SetSkeletalMesh(playerMeshAsset.Object);

	//Customize the character movement component here!
	GetCharacterMovement()->MaxWalkSpeed = Movement::PLAYER_GROUND_SPEED;
	GetCharacterMovement()->GravityScale = PlayerBasics::PLAYER_BASE_GRAVITY;
	//GetCharacterMovement()->JumpZVelocity = this->m_jumpVelocity;
	//GetCharacterMovement()->AirControl = this->m_jumpControl;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Character will look in the direction of movement
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//Character will look in the direction of the axis controlling the camera
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	this->cameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArm");
	this->cameraArm->SetupAttachment(this->RootComponent);
	this->cameraArm->TargetArmLength = 500.0f; // TODO add to CameraMovement class
	
	this->camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	this->camera->SetupAttachment(this->cameraArm);
	
	this->cameraArm->bUsePawnControlRotation = false;
	this->camera->bUsePawnControlRotation = false;

	//Set Locations and Rotations after attachments have been set
	this->cameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	//this->cameraArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	this->camera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	/*      *\
	   Grab
	\*      */

	this->grabRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GrabRoot"));
	this->grabRoot->SetRelativeLocation(FVector(2 * PlayerBasics::PLAYER_RADIUS, 0.0f, PlayerBasics::PLAYER_HEIGHT));
	this->grabRoot->SetupAttachment(skeletalMesh);

	this->grabBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GrabBox"));
	this->grabBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	this->grabBox->SetGenerateOverlapEvents(true);
	this->grabBox->OnComponentBeginOverlap.AddDynamic(this, &ACB_PlayerCharacter::OnEnterGrabBox);
	this->grabBox->OnComponentEndOverlap.AddDynamic(this, &ACB_PlayerCharacter::OnLeaveGrabBox);
	this->grabBox->SetupAttachment(this->grabRoot);
}


// Network Replication
void ACB_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	APlayerState* OwningPlayerState = this->GetPlayerState();

	if (OwningPlayerState != nullptr)
	{
		ACB_PlayerState* CBOwningPlayerState = Cast<ACB_PlayerState>(OwningPlayerState);

		if (CBOwningPlayerState != nullptr)
		{
			FString Team = CBOwningPlayerState->Team;

			if (Team.Len() >0)
			{
				UMaterialInstanceDynamic* OwningPlayerMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);

				if (Team.Equals("Blue")) {
					OwningPlayerMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Blue);
				}
				else if (Team.Equals("Yellow")) {
					OwningPlayerMaterial->SetVectorParameterValue("BodyColor", FLinearColor::Yellow);
				}

				GetMesh()->SetMaterial(0, OwningPlayerMaterial);
			}
		}
	}
}

// Called when the game starts or when spawned
void ACB_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	this->m_resetCollisionFrame = PlayerBasics::RESET_COLLISION_FRAMES + 1;

	this->m_basics.m_gameWorldRef = GetWorld();
	this->m_basics.dodgeballClassRef = DodgeballProjectileClass;
	this->m_basics.m_movement.setStartRotation(this->cameraArm->GetComponentRotation());

	//this->skeletalMesh->PlayAnimation(this->blendspace, true);
}

// Called every frame
void ACB_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	playerUpdate(DeltaTime);

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	adjustGravity(characterMovement);

	this->m_dodge.update(DeltaTime);
	this->m_throw.update(this->GetActorLocation(), this->GetActorRotation(), DeltaTime);

// START RADIUS UPDATE

	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	capsuleComponent->SetCapsuleSize(this->m_basics.m_currentRadius, this->m_basics.m_currentHeight);
	this->skeletalMesh->SetRelativeLocation(FVector(PlayerBasics::PLAYER_RADIUS - this->m_basics.m_currentRadius, 0.0f,
		-PlayerBasics::PLAYER_HEIGHT));

// END RADIUS UPDATE

	cameraUpdate();

	
	/*
	FVector groundVelocity = this->GetVelocity();
	groundVelocity.Z = 0;
	float currentSpeed = groundVelocity.Size();
	FVector idleRunBlendParams(currentSpeed, 0.0f, 0.0f);
	this->skeletalMesh->GetSingleNodeInstance()->SetBlendSpaceInput(idleRunBlendParams);*/
	
	this->m_basics.m_movement.resetInputVelocity();

}

void ACB_PlayerCharacter::playerUpdate(float deltaTime)
{
	if (Controller != nullptr)
	{
		this->SetActorLocation(this->m_basics.checkPlayerBounds(this->GetActorLocation()));
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
			this->m_throw.drop();

			this->m_basics.m_fellOff = false;
		}

		this->m_basics.m_movement.updateVelocity(this->m_basics.m_currentMobility);

		GetCharacterMovement()->Velocity = this->m_basics.m_movement.getMovementVelocity(GetCharacterMovement()->Velocity.Z);

		if (this->m_resetCollisionFrame >= 0)
		{
			if (this->m_resetCollisionFrame < PlayerBasics::RESET_COLLISION_FRAMES)
				this->m_resetCollisionFrame++;
			else if (this->m_resetCollisionFrame == PlayerBasics::RESET_COLLISION_FRAMES)
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

	this->GetCapsuleComponent()->SetRelativeRotation(playerRotation);
	this->cameraArm->SetRelativeRotation(this->m_basics.m_cameraMovement.getCameraRotation() - playerRotation);

	this->m_basics.m_currentWorldLocationZ = ((1 - PlayerBasics::WORLD_LOCATION_PROPORTION_Z) * this->GetActorLocation().Z)
		+ (PlayerBasics::WORLD_LOCATION_PROPORTION_Z * PlayerBasics::PLAYER_START_WORLD_LOCATION_Z);

	this->cameraArm->SetWorldLocation(FVector(currentLocation.X, currentLocation.Y, this->m_basics.m_currentWorldLocationZ));

	
	//this->grabBox->SetRelativeRotation(playerRotation);
	//Quick way to make sure grabBox follows model orientation
	this->grabBox->SetRelativeRotation(this->skeletalMesh->GetRelativeRotation());
}

void ACB_PlayerCharacter::adjustGravity(UCharacterMovementComponent* characterMovement)
{
	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = PlayerBasics::PLAYER_FAST_GRAVITY;
	else
		characterMovement->GravityScale = PlayerBasics::PLAYER_BASE_GRAVITY;
}

// Called to bind functionality to input
void ACB_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACB_PlayerCharacter::MoveVertical(float amount)
{
	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw + this->m_basics.m_cameraMovement.getCameraRotation().Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		this->m_basics.m_movement.addInputVector(amount * movementDirection);
	}
}

void ACB_PlayerCharacter::MoveHorizontal(float amount)
{
	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw + this->m_basics.m_cameraMovement.getCameraRotation().Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		this->m_basics.m_movement.addInputVector(amount * movementDirection);
	}
}

void ACB_PlayerCharacter::RotateCamera(float amount)
{
	this->m_basics.m_cameraMovement.updateCamera(amount);
}

void ACB_PlayerCharacter::JumpAction()
{
	this->m_dodge.onPress();
}

void ACB_PlayerCharacter::StopJumpAction()
{
	this->m_dodge.onRelease();
}

void ACB_PlayerCharacter::ShootAction() // TODO create a Dodgeball Generator
{
	this->m_throw.onPress();
}

void ACB_PlayerCharacter::StopShootAction()
{
	this->m_throw.onRelease(this->GetActorRotation());
}

void ACB_PlayerCharacter::AliveAction()
{
	this->m_basics.makeAlive();
}

// Grab

void ACB_PlayerCharacter::OnEnterGrabBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult)
{
	if (this == otherActor)
		return;	

	this->m_throw.m_grabbableList.add(Cast<IGrabbable>(otherActor));
}

void ACB_PlayerCharacter::OnLeaveGrabBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor,
	UPrimitiveComponent* otherComponent, int32 otherBodyIndex)
{
	if (this == otherActor)
		return;

	this->m_throw.m_grabbableList.remove(Cast<IGrabbable>(otherActor));
}

float ACB_PlayerCharacter::getRadius()
{
	return PlayerBasics::PLAYER_RADIUS;
}

bool ACB_PlayerCharacter::isGrabbable()
{
	return this->m_basics.getPlayerState() == PlayerBasics::PLAYER_ALIVE;
}

void ACB_PlayerCharacter::makeGrabbed()
{
	this->grabBox->SetGenerateOverlapEvents(false);
	this->m_basics.makeGrabbed();
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
	return this->m_basics.isGrounded();
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
	bool objectCatched = (this->m_throw.m_grabbableList.length() == 0) ? false : true;
	return objectCatched;
}

void ACB_PlayerCharacter::ignoreCollisionsOnThrownObject(AActor* spawnedActor)
{
	this->MoveIgnoreActorAdd(spawnedActor);
}
