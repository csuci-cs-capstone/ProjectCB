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
	//this->m_dodgeballOffset = FVector(1, 0, 0);

	this->xMovement = 0;
	this->yMovement = 0;

	this->m_startedLeap = false;
	this->m_inLeap = false;

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

	auto characterMovement = GetCharacterMovement();

	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = this->m_fastGravity;
	else
		characterMovement->GravityScale = this->m_baseGravity;

	if (this->m_startedLeap) // TODO make into roll
	{
		characterMovement->GravityScale = this->m_baseGravity / 2;
		characterMovement->AirControl = 0;

		if (!this->m_inLeap)
		{
			const FRotator controlRotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, controlRotation.Yaw, 0);

			const FVector movementDirection = (this->xMovement * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X))
				+ (this->yMovement * FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

			characterMovement->Velocity = (4 * this->m_walkSpeed) * movementDirection.GetSafeNormal();

			characterMovement->SetJumpAllowed(true);
			Jump();

			this->m_inLeap = true;
		}

		if (characterMovement->IsMovingOnGround())
		{
			this->m_startedLeap = false;
			this->m_inLeap = false;
			characterMovement->AirControl = this->m_baseAirControl;
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
	this->xMovement = amount;

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
	this->yMovement = amount;

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
	if (!this->m_startedLeap)
		this->m_startedLeap = true;
}

void ACB_PlayerCharacter::StopRunAction()
{
}

void ACB_PlayerCharacter::ShootAction()
{
	if (DodgeballClass != nullptr)
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
