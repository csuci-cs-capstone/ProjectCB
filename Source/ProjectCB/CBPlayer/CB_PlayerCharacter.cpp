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
	BaseTurnRate = 45.f; // TODO remove
	BaseLookUpRate = 45.f;

	//Customize the character movement component here!
	GetCharacterMovement()->MaxWalkSpeed = Movement::playerWalkSpeed;
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
	this->cameraArm->TargetArmLength = 500.0f;// 285.0f;
	
	 
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

	playerUpdate(DeltaTime);

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	adjustGravity(characterMovement);

	this->m_dodge.update(DeltaTime);

	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	capsuleComponent->SetCapsuleSize(25.0f, this->m_basics.m_currentSize);

	cameraUpdate();

	if (this->m_frameCounterActive)
		this->m_frameCounter++;

	// resets velocity
	this->m_basics.m_movement.m_inputVelocity.X = 0.0f;
	this->m_basics.m_movement.m_inputVelocity.Y = 0.0f;
}

void ACB_PlayerCharacter::playerUpdate(float deltaTime)
{
	this->m_basics.m_controlRotation = Controller->GetControlRotation();

	UCharacterMovementComponent* characterMovement = GetCharacterMovement();

	this->m_basics.m_grounded = characterMovement->IsMovingOnGround();

	if (this->m_basics.m_shouldJump) // TODO set jump movement to 1 to jump
	{
		characterMovement->Velocity = this->m_basics.m_velocity; // TODO remove
		characterMovement->JumpZVelocity = this->m_basics.m_jumpZVelocity;

		Jump();

		this->m_basics.m_shouldJump = false;
	}

	if(this->m_basics.m_movement.m_inputVelocity.Size() > 1)
		this->m_basics.m_movement.m_inputVelocity.Normalize();
	this->m_basics.m_movement.updateVelocity();
	this->AddMovementInput(FVector(this->m_basics.m_movement.m_currentVelocity, 0.0f)); // TODO make jump velocity
}

void ACB_PlayerCharacter::cameraUpdate()
{
	FVector currentLocation = this->cameraArm->GetComponentLocation();
	const FRotator& playerRotation = this->m_basics.m_movement.getPlayerRotation();

	this->GetCapsuleComponent()->SetRelativeRotation(playerRotation);
	this->cameraArm->SetRelativeRotation(this->m_basics.m_cameraMovement.getCameraRotation() - playerRotation);

	this->m_basics.m_currentWorldLocationZ = ((1 - PlayerBasics::worldLocationProportionZ) * this->GetActorLocation().Z)
		+ (PlayerBasics::worldLocationProportionZ * PlayerBasics::playerStartWorldLocationZ);

	this->cameraArm->SetWorldLocation(FVector(currentLocation.X, currentLocation.Y, this->m_basics.m_currentWorldLocationZ));
}

void ACB_PlayerCharacter::adjustGravity(UCharacterMovementComponent* characterMovement)
{
	if (characterMovement->Velocity.Z <= 0)
		characterMovement->GravityScale = PlayerBasics::playerFastGravity;
	else
		characterMovement->GravityScale = PlayerBasics::playerBaseGravity;
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

	amount *= this->m_basics.m_currentMobility;

	if ((Controller != NULL) && (amount != 0.0f))
	{
		const FRotator controlRotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, controlRotation.Yaw + this->m_basics.m_cameraMovement.getCameraRotation().Yaw, 0);

		const FVector movementDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		this->m_basics.m_movement.m_inputVelocity.X += amount * movementDirection.X;
		this->m_basics.m_movement.m_inputVelocity.Y += amount * movementDirection.Y;
	}
}

void ACB_PlayerCharacter::LookVertical(float amount)
{
	//AddControllerPitchInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	//AddControllerYawInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACB_PlayerCharacter::LookHorizontal(float amount) // TODO remove?
{	
	AddControllerYawInput(amount * BaseTurnRate * GetWorld()->GetDeltaSeconds());
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

void ACB_PlayerCharacter::RunAction() // TODO rename to FrameDebug
{
	// Frame counter (counts the number of frames held down for)
	this->m_frameCounterActive = true;
}

void ACB_PlayerCharacter::StopRunAction() // TODO rename to StopFrameDebug
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

		dodgeball->launch(spawnRotation.RotateVector(this->m_throwDirection));
	}
}

void ACB_PlayerCharacter::StopShootAction()
{
}
