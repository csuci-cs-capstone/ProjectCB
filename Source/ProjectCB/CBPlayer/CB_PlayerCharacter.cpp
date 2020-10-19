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

	this->m_movementX = 0;
	this->m_movementY = 0;

	this->m_mobility = 1;

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
	this->cameraArm->SetRelativeRotation(FRotator(-35.0f, 0.0f, 0.0f));
	this->camera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

float ACB_PlayerCharacter::getAnimationPoint(float x) // ranges between 0 and 1
{
	return x;// TODO adjust to sync with procedural animations (add additional parameters where needed)
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

	this->m_currentWorldLocationZ = ((1 - this->m_worldLocationProportionZ) * this->GetActorLocation().Z)
		+ (this->m_worldLocationProportionZ * g_playerStartWorldLocationZ);

	this->cameraArm->SetWorldLocation(FVector(currentLocation.X, currentLocation.Y, this->m_currentWorldLocationZ));
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
			this->m_previousSize = this->m_currentSize;

		this->m_mobility = 1; // TODO make variable

		float proportion = this->m_dodge.m_dodgeFrame / dodgeProportion(this->m_dodge.m_dodgeFramesToApex, this->m_dodge.m_diveFramesToApex);

		// TODO make rotate for dive

		if (proportion >= 1)
		{
			if (proportion >= 2)
			{
				this->m_currentSize = dodgeProportion(this->m_dodge.m_dodgeEndColliderSize, this->m_dodge.m_diveEndColliderSize);
			}
			else
			{
				proportion = getAnimationPoint(proportion - 1);

				float apexColliderSize = dodgeProportion(this->m_dodge.m_dodgeApexColliderSize, this->m_dodge.m_diveApexColliderSize);

				float endColliderSize = dodgeProportion(this->m_dodge.m_dodgeEndColliderSize, this->m_dodge.m_diveEndColliderSize);

				this->m_currentSize = ((1 - proportion) * apexColliderSize) + (proportion * endColliderSize);
			}
		}
		else
		{
			proportion = getAnimationPoint(proportion);

			float apexColliderSize = dodgeProportion(this->m_dodge.m_dodgeApexColliderSize, this->m_dodge.m_diveApexColliderSize);

			this->m_currentSize = (1 - proportion) * this->m_previousSize + proportion * apexColliderSize;
		}

		if (characterMovement->IsMovingOnGround())
		{
			this->m_dodge.m_dodgeFrame = false;
			this->m_dodge.m_dodgeCooldownFrame = true;
			this->m_previousSize = this->m_currentSize;
		}
		else
			this->m_dodge.m_dodgeFrame++;
	}
	else if (this->m_dodge.m_dodgeCooldownFrame)
	{
		this->m_mobility = 0; // TODO make variable

		short maxCooldownFrames = dodgeProportion(this->m_dodge.m_dodgeCooldownFrames, this->m_dodge.m_diveCooldownFrames);

		if (this->m_dodge.m_dodgeCooldownFrame >= maxCooldownFrames)
		{
			this->m_dodge.m_dodgeCooldownFrame = false;

			this->m_currentSize = dodgeProportion(this->m_dodge.m_dodgeCooldownColliderSize, this->m_dodge.m_diveCooldownColliderSize);

			this->m_mobility = 1; // TODO make variable
		}
		else
		{
			this->m_dodge.m_dodgeCooldownFrame++;

			float proportion = getAnimationPoint(this->m_dodge.m_dodgeCooldownFrame / (maxCooldownFrames * 1.0f));

			float colliderSize = dodgeProportion(this->m_dodge.m_dodgeCooldownColliderSize, this->m_dodge.m_diveCooldownColliderSize);

			this->m_currentSize = ((1 - proportion) * this->m_previousSize) + (proportion * colliderSize);
		}
	}
	else if (this->m_duck.m_frame) // TODO remove booleans and just use frames
	{
		if (this->m_duck.m_frame == 1)
			this->m_previousSize = this->m_currentSize;

		if (this->m_duck.m_frame >= this->m_duck.m_startupFrames)
		{
			if (this->m_duck.m_frame >= (this->m_duck.m_startupFrames + this->m_duck.m_actionFrames))
			{
				this->m_mobility = 0; // TODO make variable

				this->m_currentSize = this->m_duck.m_colliderSize;
			}
			else
			{
				float proportion = (this->m_duck.m_frame - this->m_duck.m_startupFrames) / (this->m_duck.m_actionFrames * 1.0f);

				proportion = getAnimationPoint(proportion);

				this->m_currentSize = (1 - proportion) * this->m_previousSize + proportion * this->m_duck.m_colliderSize;
			}
		}

		this->m_duck.m_frame++;
	}

	capsuleComponent->SetCapsuleSize(25.0f, this->m_currentSize);
}

float ACB_PlayerCharacter::dodgeProportion(float dodgeValue, float diveValue)
{
	return (1 - this->m_dodge.m_diveProportion) * dodgeValue + this->m_dodge.m_diveProportion * diveValue;
}

// Called to bind functionality to input
void ACB_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACB_PlayerCharacter::MoveVertical(float amount)
{
	this->m_movementX = amount;

	amount *= this->m_mobility;

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
	this->m_movementY = amount;

	amount *= this->m_mobility;

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

			FVector direction = (this->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

			this->m_dodge.m_diveProportion = direction.Size();

			characterMovement->Velocity = (this->m_dodge.m_diveProportion * this->m_dodge.m_diveHorizontalVelocity) * direction;

			characterMovement->JumpZVelocity = dodgeProportion(this->m_dodge.m_dodgeVelocity, this->m_dodge.m_diveVerticalVelocity);
			
			characterMovement->AirControl = dodgeProportion(this->m_dodge.m_dodgeControl, this->m_dodge.m_diveControl);

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
