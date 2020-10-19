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
	this->m_frameCounterActive = false;
	this->m_frameCounter = 0;

	// General Variables

	this->m_basics.m_movementX = 0;
	this->m_basics.m_movementY = 0;

	this->m_basics.m_mobility = 1;

	// Dodge Variables

	this->m_duck.m_frame = false;
	this->m_dodge.m_dodgeFrame = false;
	this->m_dodge.m_dodgeCooldownFrame = false;

	// Other

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//GetCapsuleComponent()->InitCapsuleSize(28.0f, 50.0f);

	GetCapsuleComponent()->InitCapsuleSize(25.0f, 50.0f);

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	staticMesh->SetupAttachment(RootComponent);

	//TODO Make run and walk speed, hook up functions from input and add them to movement
	//TODO Make some fields UPROPERTIES so we can edit them
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//Customize the character movement component here!
	GetCharacterMovement()->MaxWalkSpeed = g_playerWalkSpeed;
	GetCharacterMovement()->GravityScale = g_playerBaseGravity;
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
	this->cameraArm->TargetArmLength = 285.0f;
	
	 
	this->camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	this->camera->SetupAttachment(cameraArm);
	
	this->cameraArm->bUsePawnControlRotation = false;
	this->camera->bUsePawnControlRotation = false;

	//Set Locations and Rotations after attachments have been set
	this->cameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	this->cameraArm->SetRelativeRotation(FRotator(-20.0f, 0.0f, 0.0f));
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

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	adjustGravity(characterMovement);

	dodgeUpdate(characterMovement);

	cameraUpdate();

	if (this->m_frameCounterActive)
		this->m_frameCounter++;
}

void ACB_PlayerCharacter::cameraUpdate()
{
	FVector currentLocation = this->cameraArm->GetComponentLocation();

	FVector actorLocation = this->GetActorLocation();

	this->m_basics.m_currentWorldLocationZ = ((1 - this->m_basics.m_worldLocationProportionZ) * actorLocation.Z)
		+ (this->m_basics.m_worldLocationProportionZ * g_playerStartWorldLocationZ);

	this->cameraArm->SetWorldLocation(FVector(currentLocation.X, actorLocation.Y + 100, this->m_basics.m_currentWorldLocationZ));
}

void ACB_PlayerCharacter::adjustGravity(UCharacterMovementComponent* characterMovement)
{
	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = g_playerFastGravity;
	else
		characterMovement->GravityScale = g_playerBaseGravity;
}

void ACB_PlayerCharacter::dodgeUpdate(UCharacterMovementComponent* characterMovement)
{
	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();

	if (this->m_dodge.m_dodgeFrame)
	{
		if (this->m_dodge.m_dodgeFrame == 1)
			this->m_basics.m_previousSize = this->m_basics.m_currentSize;

		this->m_basics.m_mobility = 1; // TODO make variable

		float proportion = this->m_dodge.m_dodgeFrame / this->m_dodge.dodgeProportion(Dodge_Release::dodgeFramesToApex,
			Dodge_Release::diveFramesToApex);

		// TODO make rotate for dive

		if (proportion >= 1)
		{
			if (proportion >= 2)
			{
				this->m_basics.m_currentSize = this->m_dodge.dodgeProportion(Dodge_Release::dodgeEndColliderSize,
					Dodge_Release::diveEndColliderSize);
			}
			else
			{
				proportion = this->m_basics.getAnimationPoint(proportion - 1);

				float apexColliderSize = this->m_dodge.dodgeProportion(Dodge_Release::dodgeApexColliderSize,
					Dodge_Release::diveApexColliderSize);

				float endColliderSize = this->m_dodge.dodgeProportion(Dodge_Release::dodgeEndColliderSize,
					Dodge_Release::diveEndColliderSize);

				this->m_basics.m_currentSize = ((1 - proportion) * apexColliderSize) + (proportion * endColliderSize);
			}
		}
		else
		{
			proportion = this->m_basics.getAnimationPoint(proportion);

			float apexColliderSize = this->m_dodge.dodgeProportion(Dodge_Release::dodgeApexColliderSize,
				Dodge_Release::diveApexColliderSize);

			this->m_basics.m_currentSize = (1 - proportion) * this->m_basics.m_previousSize + proportion * apexColliderSize;
		}

		if (characterMovement->IsMovingOnGround())
		{
			this->m_dodge.m_dodgeFrame = false;
			this->m_dodge.m_dodgeCooldownFrame = true;
			this->m_basics.m_previousSize = this->m_basics.m_currentSize;
		}
		else
			this->m_dodge.m_dodgeFrame++;
	}
	else if (this->m_dodge.m_dodgeCooldownFrame)
	{
		this->m_dodge.dodgeCooldownUpdate(this->m_basics);
	}
	else if (this->m_duck.m_frame) // TODO remove booleans and just use frames
	{
		if (this->m_duck.m_frame == 1)
			this->m_basics.m_previousSize = this->m_basics.m_currentSize;

		if (this->m_duck.m_frame >= Dodge_Hold::startupFrames)
		{
			if (this->m_duck.m_frame >= (Dodge_Hold::startupFrames + Dodge_Hold::actionFrames))
			{
				this->m_basics.m_mobility = 0; // TODO make variable

				this->m_basics.m_currentSize = Dodge_Hold::colliderSize;
			}
			else
			{
				float proportion = (this->m_duck.m_frame - Dodge_Hold::startupFrames) / (Dodge_Hold::actionFrames * 1.0f);

				proportion = this->m_basics.getAnimationPoint(proportion);

				this->m_basics.m_currentSize = (1 - proportion) * this->m_basics.m_previousSize
					+ proportion * Dodge_Hold::colliderSize;
			}
		}

		this->m_duck.m_frame++;
	}

	capsuleComponent->SetCapsuleSize(25.0f, this->m_basics.m_currentSize);
}

//void ACB_PlayerCharacter::dodgeCooldownUpdate(PlayerBasics playerBasics)
//{
//	playerBasics.m_mobility = 0; // TODO make variable
//
//	short maxCooldownFrames = this->m_dodge.dodgeProportion(Dodge_Release::dodgeCooldownFrames,
//		Dodge_Release::diveCooldownFrames);
//
//	if (this->m_dodge.m_dodgeCooldownFrame >= maxCooldownFrames)
//	{
//		this->m_dodge.m_dodgeCooldownFrame = false;
//
//		playerBasics.m_currentSize = this->m_dodge.dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
//			Dodge_Release::diveCooldownColliderSize);
//
//		playerBasics.m_mobility = 1; // TODO make variable
//	}
//	else
//	{
//		this->m_dodge.m_dodgeCooldownFrame++;
//
//		float proportion = playerBasics.getAnimationPoint(this->m_dodge.m_dodgeCooldownFrame / (maxCooldownFrames * 1.0f));
//
//		float colliderSize = this->m_dodge.dodgeProportion(Dodge_Release::dodgeCooldownColliderSize,
//			Dodge_Release::diveCooldownColliderSize);
//
//		this->m_basics.m_currentSize = ((1 - proportion) * playerBasics.m_previousSize) + (proportion * colliderSize);
//	}
//}

// Called to bind functionality to input
void ACB_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACB_PlayerCharacter::MoveVertical(float amount)
{
	this->m_basics.m_movementX = amount;

	amount *= this->m_basics.m_mobility;

	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(movementDirection, amount);
	}
}

void ACB_PlayerCharacter::MoveHorizontal(float amount)
{
	this->m_basics.m_movementY = amount;

	amount *= this->m_basics.m_mobility;

	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(movementDirection, amount);
	}
}

void ACB_PlayerCharacter::LookVertical(float amount)
{
	AddControllerPitchInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	//AddControllerYawInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACB_PlayerCharacter::LookHorizontal(float amount)
{	
	AddControllerYawInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACB_PlayerCharacter::JumpAction()
{
	if (!this->m_duck.m_frame)
		this->m_duck.m_frame = true;
}

void ACB_PlayerCharacter::StopJumpAction()
{
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	if (this->m_duck.m_frame)
	{
		if (!this->m_dodge.m_dodgeCooldownFrame && characterMovement->IsMovingOnGround())
		{
			this->m_dodge.m_dodgeFrame = true;

			const FRotator controlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, controlRotation.Yaw, 0);

			FVector direction = (this->m_basics.m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->m_basics.m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

			this->m_dodge.m_diveProportion = direction.Size();

			characterMovement->Velocity = (this->m_dodge.m_diveProportion * Dodge_Release::diveHorizontalVelocity)
				* direction;

			characterMovement->JumpZVelocity = this->m_dodge.dodgeProportion(Dodge_Release::dodgeVelocity,
				Dodge_Release::diveVerticalVelocity);
			
			characterMovement->AirControl = this->m_dodge.dodgeProportion(Dodge_Release::dodgeControl,
				Dodge_Release::diveControl);

			this->Jump();
		}

		this->m_duck.m_frame = false;
	}
}

void ACB_PlayerCharacter::RunAction()
{
	// Frame counter (counts the number of frames held down for)
	this->m_frameCounterActive = true;
}

void ACB_PlayerCharacter::StopRunAction()
{
	this->m_frameCounterActive = false;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Frames: %d"), this->m_frameCounter));
	this->m_frameCounter = 0;
}

void ACB_PlayerCharacter::ShootAction()
{
	if (this->DodgeballProjectileClass != nullptr)
	{
		FActorSpawnParameters spawnParameters;

		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParameters.bNoFail = true;
		spawnParameters.Owner = this;
		spawnParameters.Instigator = this;

		FTransform spawnTransform;

		//Scale forward vector by 20.0f so it won't clip into the capsule collider
		FVector spawnLocation = GetActorForwardVector() * 125.0f + FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 35.0f);

		FRotator spawnRotation(Controller->GetControlRotation().Pitch, GetActorRotation().Yaw, 0);

		spawnTransform.SetLocation(spawnLocation);
		spawnTransform.SetScale3D(FVector(0.5f));

		auto dodgeball = GetWorld()->SpawnActor<ACB_DodgeballProjectile>(this->DodgeballProjectileClass, spawnTransform, spawnParameters);

		dodgeball->launch(Controller->GetControlRotation().RotateVector(this->m_throwDirection));
	}
}

void ACB_PlayerCharacter::StopShootAction()
{
}
