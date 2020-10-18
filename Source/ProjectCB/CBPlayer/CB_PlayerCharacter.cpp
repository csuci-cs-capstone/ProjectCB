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

	this->m_duckFrame = 0;
	this->m_dodgeFrame = 0;

	this->m_dodgeCooldownStarted = false;

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
	GetCharacterMovement()->MaxWalkSpeed = this->m_walkSpeed;
	GetCharacterMovement()->GravityScale = this->m_baseGravity;
	//GetCharacterMovement()->JumpZVelocity = this->m_jumpVelocity;
	//GetCharacterMovement()->AirControl = this->m_jumpControl;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Character will look in the direction of movement
	//GetCharacterMovement()->bOrientRotationToMovement = true;
	//Character will look in the direction of the axis controlling the camera
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	cameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArm");
	cameraArm->SetupAttachment(staticMesh);
	cameraArm->SetRelativeLocation(FVector(0.0f, 60.0f, 0.0f));
	cameraArm->TargetArmLength = 285.0f;
	cameraArm->SetWorldRotation(FRotator(-60.0f, 0.0f, 0.0f));
	 
	camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	camera->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
	camera->SetupAttachment(cameraArm);

	camera->bUsePawnControlRotation = false;
	cameraArm->bUsePawnControlRotation = false;
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

	GetCapsuleComponent()->SetCapsuleSize(25.0f, this->m_currentSize);

	if (this->m_frameCounterActive)
		this->m_frameCounter++;
}

void ACB_PlayerCharacter::adjustGravity(UCharacterMovementComponent* characterMovement)
{
	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = this->m_fastGravity;
	else
		characterMovement->GravityScale = this->m_baseGravity;
}

void ACB_PlayerCharacter::dodgeUpdate(UCharacterMovementComponent* characterMovement)
{
	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();

	ACB_PlayerCharacter* player = this;

	if (this->m_dodgeFrame)
	{
		if (this->m_dodgeFrame == 1)
			player->m_previousSize = player->m_currentSize;

		float proportion = this->m_dodgeFrame / dodgeProportion(this->m_dodgeFramesToApex, this->m_diveFramesToApex);

		// TODO make rotate for dive

		if (proportion < 2)
		{
			float apexColliderSize = dodgeProportion(this->m_dodgeApexColliderSize, this->m_diveApexColliderSize);

			if (proportion < 1)
			{
				proportion = player->getAnimationPoint(proportion);

				player->m_currentSize = ((1 - proportion) * player->m_previousSize) + (proportion * apexColliderSize);
			}
			else
			{
				float endColliderSize = dodgeProportion(this->m_dodgeEndColliderSize, this->m_diveEndColliderSize);

				proportion = player->getAnimationPoint(proportion - 1);

				this->m_currentSize = ((1 - proportion) * apexColliderSize) + (proportion * endColliderSize);
			}

			this->m_dodgeFrame++;
		}
		else
			player->m_currentSize = dodgeProportion(this->m_dodgeEndColliderSize, this->m_diveEndColliderSize);

		if (characterMovement->IsMovingOnGround())
		{
			this->m_dodgeCooldownStarted = true;
			this->m_dodgeFrame = 1;
			this->m_previousSize = this->m_currentSize;
		}
	}
	else if (this->m_cooldownFrame)
	{
		player->m_mobility = dodgeProportion(this->m_jumpCooldownMobility, this->m_leapCooldownMobility);

		short maxCooldownFrames = dodgeProportion(this->m_jumpCooldownFrames, this->m_leapCooldownFrames);

		if (this->m_dodgeFrame >= maxCooldownFrames)
		{
			this->m_dodgeFrame = 0;

			player->m_mobility = 1;

			player->m_currentSize = dodgeProportion(this->m_jumpCooldownColliderSize, this->m_leapCooldownColliderSize);
		}
		else
		{
			this->m_cooldownFrame++;

			float proportion = player->getAnimationPoint(this->m_cooldownFrame / (maxCooldownFrames * 1.0f));

			float colliderSize = dodgeProportion(this->m_jumpCooldownColliderSize, this->m_leapCooldownColliderSize);

			player->m_currentSize = ((1 - proportion) * player->m_previousSize) + (proportion * colliderSize);
		}
	}
	else if (this->m_duckFrame)
	{
		if (this->m_duckFrame == 1)
			player->m_previousSize = player->m_currentSize;

		if (this->m_duckFrame >= this->m_duckStartupFrames)
		{
			if (this->m_duckFrame >= (this->m_duckStartupFrames + this->m_duckActionFrames))
			{
				this->m_mobility = 0;
				player->m_currentSize = this->m_duckColliderSize;
			}
			else
			{
				float proportion = getAnimationPoint((this->m_duckFrame - this->m_duckStartupFrames) / (this->m_duckActionFrames * 1.0f));

				player->m_currentSize = (1 - proportion) * player->m_previousSize + proportion * this->m_duckColliderSize;
			}
		}

		this->m_duckFrame++;
	}
}

float ACB_PlayerCharacter::dodgeProportion(float dodgeValue, float diveValue)
{
	return (1 - this->m_diveProportion) * dodgeValue + this->m_diveProportion * diveValue;
}

// Called to bind functionality to input
void ACB_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACB_PlayerCharacter::MoveVertical(float amount)
{
	this->m_movementX = this->m_mobility * amount;

	if ((Controller != NULL) && (m_movementX != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(movementDirection, m_movementX);
	}
}

void ACB_PlayerCharacter::MoveHorizontal(float amount)
{
	this->m_movementY = this->m_mobility * amount;

	if ((Controller != NULL) && (this->m_movementY != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(movementDirection, this->m_movementY);
	}
}

void ACB_PlayerCharacter::LookVertical(float amount)
{
	//AddControllerPitchInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	AddControllerYawInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACB_PlayerCharacter::LookHorizontal(float amount)
{	
	AddControllerYawInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACB_PlayerCharacter::JumpAction()
{
	if (!this->m_duckFrame)
		this->m_duckFrame = 1;
}

void ACB_PlayerCharacter::StopJumpAction()
{
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	if (this->m_duckFrame)
	{
		// Initializes Jump
		if (!this->m_dodgeCooldownStarted && characterMovement->IsMovingOnGround())
		{
			const FRotator controlRotation = Controller->GetControlRotation(); // TODO should factor in acceleration
			const FRotator YawRotation(0, controlRotation.Yaw, 0);

			FVector direction = (this->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

			this->m_diveProportion = direction.Size();
			this->m_mobility = dodgeProportion(this->m_dodgeActionMobility, this->m_diveActionMobility);

			characterMovement->Velocity = (this->m_diveProportion * this->m_diveHorizontalVelocity) * direction;
			characterMovement->JumpZVelocity = dodgeProportion(this->m_dodgeVelocity, this->m_diveVerticalVelocity);

			this->Jump();

			this->m_dodgeFrame = 1;
		}

		this->m_duckFrame = 0;
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
