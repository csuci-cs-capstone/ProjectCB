// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerCharacter.h"
#include "ProjectCB/CBObjects/CB_DodgeballProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s"), text));

// Sets default values
ACB_PlayerCharacter::ACB_PlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(25.0f, 50.0f); // TODO create default size

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	staticMesh->SetupAttachment(RootComponent);

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
	this->cameraArm->SetupAttachment(staticMesh);
	this->cameraArm->TargetArmLength = 500.0f; // TODO add to CameraMovement class
	
	this->camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	this->camera->SetupAttachment(cameraArm);
	
	this->cameraArm->bUsePawnControlRotation = false;
	this->camera->bUsePawnControlRotation = false;

	//Set Locations and Rotations after attachments have been set
	this->cameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	//this->cameraArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
	this->camera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ACB_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACB_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	playerUpdate(DeltaTime);

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	adjustGravity(characterMovement);

	this->m_dodge.update(DeltaTime);

	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	capsuleComponent->SetCapsuleSize(25.0f, this->m_basics.m_currentSize);

	cameraUpdate();

	// resets velocity
	this->m_basics.m_movement.m_inputVelocity.X = 0.0f;
	this->m_basics.m_movement.m_inputVelocity.Y = 0.0f;
}

void ACB_PlayerCharacter::playerUpdate(float deltaTime)
{
	this->m_basics.m_controlRotation = Controller->GetControlRotation();

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	this->m_basics.updateGroundState(characterMovement->IsMovingOnGround());

	if (this->m_basics.m_shouldJump) // TODO set jump movement to 1 to jump
	{
		//characterMovement->Velocity = this->m_basics.m_velocity; // TODO remove
		characterMovement->JumpZVelocity = this->m_basics.m_jumpZVelocity;

		// TODO make use leap velocity and make it so that mobility is accounted for in deceleration

		Jump();

		this->m_basics.m_shouldJump = false;
	}

	if(this->m_basics.m_movement.m_inputVelocity.Size() > 1)
		this->m_basics.m_movement.m_inputVelocity.Normalize();

	this->m_basics.m_movement.updateVelocity(this->m_basics.m_currentMobility);

	GetCharacterMovement()->MaxWalkSpeed = this->m_basics.m_movement.getSpeed();

	this->AddMovementInput(FVector(this->m_basics.m_movement.m_currentVelocity, 0.0f)); // TODO make jump velocity
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
	this->m_basics.m_movementX = amount;

	amount *= this->m_basics.m_currentMobility;

	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw + this->m_basics.m_cameraMovement.getCameraRotation().Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		this->m_basics.m_movement.m_inputVelocity.X += amount * movementDirection.X;
		this->m_basics.m_movement.m_inputVelocity.Y += amount * movementDirection.Y;
	}
}

void ACB_PlayerCharacter::MoveHorizontal(float amount)
{
	this->m_basics.m_movementY = amount;

	amount *= this->m_basics.m_currentMobility; // TODO do all mobility checks in movement

	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw + this->m_basics.m_cameraMovement.getCameraRotation().Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		this->m_basics.m_movement.m_inputVelocity.X += amount * movementDirection.X;
		this->m_basics.m_movement.m_inputVelocity.Y += amount * movementDirection.Y;
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
	// TODO add to Throw::onPress()

	if (this->DodgeballProjectileClass)
	{
		FActorSpawnParameters spawnParameters;

		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParameters.bNoFail = true;
		spawnParameters.Owner = this;
		spawnParameters.Instigator = this;

		FTransform spawnTransform;

		//Scale forward vector by 20.0f so it won't clip into the capsule collider
		FVector spawnLocation = GetActorForwardVector() * 125.0f
			+ FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);

		const FRotator& cameraRotation = this->m_basics.m_cameraMovement.getCameraRotation();
		const FRotator& playerRotation = this->m_basics.m_cameraMovement.getCameraRotation();

		FRotator spawnRotation(Controller->GetControlRotation().Pitch,
			GetActorRotation().Yaw + cameraRotation.Yaw - playerRotation.Yaw, 0);

		spawnTransform.SetLocation(spawnLocation);
		spawnTransform.SetScale3D(FVector(0.5f));

		auto dodgeball = GetWorld()->SpawnActor<ACB_DodgeballProjectile>(this->DodgeballProjectileClass,
			spawnTransform, spawnParameters);

		dodgeball->launch(spawnRotation.RotateVector(Throw::THROW_DIRECTION));
	}
}

void ACB_PlayerCharacter::StopShootAction()
{
}
