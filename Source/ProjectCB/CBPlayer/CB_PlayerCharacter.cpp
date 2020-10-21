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
	this->m_dodgeHold.m_playerBasics = &(this->m_basics);

	this->m_frameCounterActive = false; // FOR DEBUGGING ONLY (TODO remove)
	this->m_frameCounter = 0; // FOR DEBUGGING ONLY (TODO remove)

	// Other

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(25.0f, 50.0f); // TODO create default size

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	staticMesh->SetupAttachment(RootComponent);

	//TODO Make run and walk speed, hook up functions from input and add them to movement
	//TODO Make some fields UPROPERTIES so we can edit them
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	//Customize the character movement component here!
	GetCharacterMovement()->MaxWalkSpeed = PlayerBasics::playerWalkSpeed;
	GetCharacterMovement()->GravityScale = PlayerBasics::playerBaseGravity;
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

	dodgeUpdate(characterMovement, DeltaTime);

	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	capsuleComponent->SetCapsuleSize(25.0f, this->m_basics.m_currentSize);

	cameraUpdate();

	if (this->m_frameCounterActive)
		this->m_frameCounter++;
}

void ACB_PlayerCharacter::cameraUpdate()
{
	FVector currentLocation = this->cameraArm->GetComponentLocation();

	FVector actorLocation = this->GetActorLocation();

	this->m_basics.m_currentWorldLocationZ = ((1 - PlayerBasics::worldLocationProportionZ) * actorLocation.Z)
		+ (PlayerBasics::worldLocationProportionZ * PlayerBasics::playerStartWorldLocationZ);

	this->cameraArm->SetWorldLocation(FVector(currentLocation.X, actorLocation.Y + 100, this->m_basics.m_currentWorldLocationZ));
}

void ACB_PlayerCharacter::adjustGravity(UCharacterMovementComponent* characterMovement)
{
	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = PlayerBasics::playerFastGravity;
	else
		characterMovement->GravityScale = PlayerBasics::playerBaseGravity;
}

void ACB_PlayerCharacter::dodgeUpdate(UCharacterMovementComponent* characterMovement, float deltaTime)
{
	if (this->m_dodgeRelease.m_dodgeFrame)
		this->m_dodgeRelease.dodgeActionUpdate(this->m_basics, characterMovement->IsMovingOnGround());

	else if (this->m_dodgeRelease.m_dodgeCooldownFrame)
		this->m_dodgeRelease.dodgeCooldownUpdate(this->m_basics);

	else if (this->m_dodgeHold.m_frame) // TODO make should start?
		//this->m_dodgeHold.start();

	this->m_dodgeHold.update(deltaTime);
}

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
	if (!this->m_dodgeHold.m_frame)
		this->m_dodgeHold.m_frame = true;
}

void ACB_PlayerCharacter::StopJumpAction()
{
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	if (this->m_dodgeHold.m_frame)
	{
		if (!this->m_dodgeRelease.m_dodgeCooldownFrame && characterMovement->IsMovingOnGround())
		{
			this->m_dodgeRelease.m_dodgeFrame = true;

			const FRotator controlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, controlRotation.Yaw, 0);

			FVector direction = (this->m_basics.m_movementX * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->m_basics.m_movementY * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			direction = direction.Size() > 1 ? direction.GetUnsafeNormal() : direction;

			this->m_dodgeRelease.m_diveProportion = direction.Size();

			characterMovement->Velocity = (this->m_dodgeRelease.m_diveProportion * Dodge_Release::diveHorizontalVelocity)
				* direction;

			characterMovement->JumpZVelocity = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeVelocity,
				Dodge_Release::diveVerticalVelocity);
			
			characterMovement->AirControl = this->m_dodgeRelease.dodgeProportion(Dodge_Release::dodgeControl,
				Dodge_Release::diveControl);

			this->Jump();
		}

		this->m_dodgeHold.m_frame = false;
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
	// TODO make into separate class
		// TODO make based of player movement direction?

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
