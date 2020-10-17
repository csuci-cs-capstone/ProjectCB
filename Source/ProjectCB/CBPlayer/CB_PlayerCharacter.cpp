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
	// General Variables

	this->m_movementX = 0;
	this->m_movementY = 0;

	this->m_canMove = true;

	// Dodge Variables

	this->m_duckFrame = 0;
	this->m_dodgeFrame = 0;

	this->m_ducked = false;
	this->m_dodged = false;
	this->m_dived = false;
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
	cameraArm->TargetArmLength = 285.0f;
	
	 
	camera = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	camera->SetupAttachment(cameraArm);
	
	cameraArm->bUsePawnControlRotation = false;
	camera->bUsePawnControlRotation = false;

	//Set Locations and Rotations after attachments have been set
	cameraArm->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	cameraArm->SetRelativeRotation(FRotator(-25.0f, 0.0f, 0.0f));
	camera->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
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

	// TODO combine dodge and dive

	if (this->m_dodged)
	{
		// TODO update size dynamically
		capsuleComponent->SetCapsuleSize(25.0f, 50.0f); // TODO make it shrink down into jump
		// TODO rotate capsule and stuff according to dodge dive

		this->m_canMove = true;

		if (characterMovement->IsMovingOnGround())
			this->m_dodgeCooldownStarted = true;

		if (this->m_dodgeCooldownStarted)
		{
			//capsuleComponent->SetCapsuleSize(25.0f, 25.0f);
			this->m_canMove = false;

			this->m_dodgeFrame++;

			if (this->m_dodgeFrame >= this->m_diveCooldownFrames)
			{
				capsuleComponent->SetCapsuleSize(25.0f, 50.0f); // TODO make based on relationship

				this->m_canMove = true;
				this->m_dodgeFrame = 0;
				this->m_dodged = false;
				this->m_dodgeCooldownStarted = false;
			}
		}
	}
	else if (this->m_ducked)
	{
		this->m_duckFrame++;

		if (this->m_duckFrame >= this->m_duckStartupFrames)
		{
			capsuleComponent->SetCapsuleSize(25.0f, 25.0f); // TODO make it go towards based on frame data
			this->m_canMove = false;
		}
	}
}

// Called to bind functionality to input
void ACB_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACB_PlayerCharacter::MoveVertical(float amount)
{
	this->m_movementX = amount;

	if (this->m_canMove && (Controller != NULL) && (amount != 0.0f))
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

	if (this->m_canMove && (Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(movementDirection, amount);
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
	if (!this->m_ducked)
		this->m_duckFrame = 0;

	this->m_ducked = true;
}

void ACB_PlayerCharacter::StopJumpAction()
{
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	if (this->m_ducked)
	{
		if (characterMovement->IsMovingOnGround())
		{
			this->m_dodged = true;

			const FRotator controlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, controlRotation.Yaw, 0);

			FVector direction = (this->m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

			this->m_diveAmount = direction.Size();

			characterMovement->Velocity = (this->m_diveHorizontalVelocity * this->m_diveAmount) * direction;

			characterMovement->JumpZVelocity = this->m_dodgeVelocity * (1 - this->m_diveAmount)
				+ this->m_diveVerticalVelocity * this->m_diveAmount;
			
			characterMovement->AirControl = this->m_dodgeControl * (1 - this->m_diveAmount)
				+ this->m_diveControl * this->m_diveAmount;

			this->Jump();
		}

		this->m_ducked = false;
	}
}

void ACB_PlayerCharacter::RunAction()
{

}

void ACB_PlayerCharacter::StopRunAction()
{
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
