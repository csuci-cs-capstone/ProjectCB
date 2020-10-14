// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerCharacter.h"
#include "ProjectCB/CBObjects/CB_Dodgeball.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("%s"), text));

// Sets default values
ACB_PlayerCharacter::ACB_PlayerCharacter()
{
	this->xMovement = 0;
	this->yMovement = 0;
	
	this->m_leapFrame = 0;
	this->m_canLeap = true;
	this->m_isLeaping = false;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(28.0f, 50.0f);

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
	GetCharacterMovement()->JumpZVelocity = this->m_jumpVelocity;
	GetCharacterMovement()->AirControl = this->m_baseAirControl;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	//Character will look in the direction of movement
	GetCharacterMovement()->bOrientRotationToMovement = true;

	cameraArm = CreateDefaultSubobject<USpringArmComponent>("CameraSpringArm");
	cameraArm->SetupAttachment(staticMesh);
	cameraArm->TargetArmLength = 185.0f;

	camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	camera->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
	camera->SetupAttachment(cameraArm);

	camera->bUsePawnControlRotation = false;
	cameraArm->bUsePawnControlRotation = true;
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

	updateLeap(characterMovement);
}

void ACB_PlayerCharacter::adjustGravity(UCharacterMovementComponent* characterMovement)
{
	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = this->m_fastGravity;
	else
		characterMovement->GravityScale = this->m_baseGravity;
}

void ACB_PlayerCharacter::updateLeap(UCharacterMovementComponent* characterMovement)
{
	if (this->m_isLeaping)
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw, 0);

		const FVector movementDirection = (this->xMovement * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
			+ (this->yMovement * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		FVector direction = movementDirection.GetSafeNormal();

		if (direction.IsNearlyZero())
			endLeap(characterMovement);
		else
			leap(characterMovement, direction);

		if (this->m_leapFrame >= this->m_maxLeapFrames)
			endLeap(characterMovement);
		else
			this->m_leapFrame++;
	}
	else if (!this->m_canLeap)
	{
		if (this->m_leapFrame >= this->m_leapCooldownFrames)
		{
			this->m_canLeap = true;
			this->m_leapFrame = 0;
		}
		else
			this->m_leapFrame++;
	}
}

void ACB_PlayerCharacter::leap(UCharacterMovementComponent* characterMovement, const FVector& direction)
{
	if (this->m_canLeap)
	{
		this->m_actionVelocity = this->m_leapSpeed * direction;
		this->m_canLeap = false;
	}

	characterMovement->GravityScale = this->m_baseGravity;
	characterMovement->AirControl = 0;
	characterMovement->Velocity = this->m_actionVelocity;

	float currentMagnitude = this->m_actionVelocity.Size2D();
	float newMagnitude = currentMagnitude - this->m_leapDeceleration;

	this->m_actionVelocity.X *= newMagnitude / currentMagnitude;
	this->m_actionVelocity.Y *= newMagnitude / currentMagnitude;
}

void ACB_PlayerCharacter::endLeap(UCharacterMovementComponent* characterMovement)
{
	characterMovement->Velocity = FVector(0, 0, 0);
	characterMovement->GravityScale = this->m_baseGravity;
	characterMovement->AirControl = this->m_baseAirControl;
	this->m_isLeaping = false;
	this->m_leapFrame = 0;
}

// Called to bind functionality to input
void ACB_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACB_PlayerCharacter::MoveVertical(float amount)
{
	if (!this->m_isLeaping)
	{
		this->xMovement = amount;

		if ((Controller != NULL) && (amount != 0.0f))
		{
			const FRotator controlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, controlRotation.Yaw, 0);

			const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			AddMovementInput(movementDirection, amount);
		}
	}
	else
		this->xMovement = ((1 - this->m_leapControl) * this->xMovement) + (this->m_leapControl * amount);
}

void ACB_PlayerCharacter::MoveHorizontal(float amount)
{
	if (!this->m_isLeaping)
	{
		this->yMovement = amount;

		if ((Controller != NULL) && (amount != 0.0f))
		{
			const FRotator controlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, controlRotation.Yaw, 0);

			const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			AddMovementInput(movementDirection, amount);
		}
	}
	else
		this->yMovement = ((1 - this->m_leapControl) * this->yMovement) + (this->m_leapControl * amount);
}

void ACB_PlayerCharacter::LookVertical(float amount)
{
	AddControllerPitchInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACB_PlayerCharacter::LookHorizontal(float amount)
{
	AddControllerYawInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACB_PlayerCharacter::JumpAction()
{
	Jump();
}

void ACB_PlayerCharacter::StopJumpAction()
{
	StopJumping();
}

void ACB_PlayerCharacter::RunAction()
{
	if (this->m_canLeap)
		this->m_isLeaping = true;
}

void ACB_PlayerCharacter::StopRunAction()
{
}

void ACB_PlayerCharacter::ShootAction()
{
	if (this->DodgeballClass != nullptr && !this->m_isLeaping)
	{
		FActorSpawnParameters spawnParameters;

		spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		spawnParameters.bNoFail = true;
		spawnParameters.Owner = this;
		spawnParameters.Instigator = this;

		FTransform spawnTransform;

		FVector location = GetActorLocation() + Controller->GetControlRotation().RotateVector(FVector(75, 0, 0));

		spawnTransform.SetLocation(location);
		spawnTransform.SetRotation(GetActorRotation().Quaternion());
		spawnTransform.SetScale3D(FVector(0.5f));

		auto dodgeball = GetWorld()->SpawnActor<class ACB_Dodgeball>(DodgeballClass, spawnTransform, spawnParameters);

		dodgeball->FireInDirection(Controller->GetControlRotation().RotateVector(this->m_throwDirection));
	}
}

void ACB_PlayerCharacter::StopShootAction()
{
}
