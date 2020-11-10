// Fill out your copyright notice in the Description page of Project Settings.


#include "CB_PlayerCharacter.h"
#include "ProjectCB/CBObjects/CB_DodgeballProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
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

	this->skeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMeshComponent");
	this->skeletalMesh->SetupAttachment(RootComponent);

	//staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	//staticMesh->SetupAttachment(RootComponent);

	//LOAD in player/ghost models from uassets
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> playerMeshAsset(TEXT("SkeletalMesh'/Game/PlayerBP/PrototypeSkeletalModel/Character1Asset.Character1Asset'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> ghostMeshAsset(TEXT("SkeletalMesh'/Game/PlayerBP/PrototypeGhostModel/GhostSkeletal/GhostPrototypeModel.GhostPrototypeModel_GhostPrototypeModel'"));
	
	this->m_basics.m_playerModel = playerMeshAsset.Object;
	this->m_basics.m_ghostModel = ghostMeshAsset.Object;
	this->m_basics.m_playerSkeletalMeshComponent = skeletalMesh;

	//staticMesh->SetStaticMesh(playerMeshAsset.Object);
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
	this->cameraArm->SetupAttachment(skeletalMesh);
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
	this->m_throw.update(this->GetActorLocation(), this->GetActorRotation(), DeltaTime);

	UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
	capsuleComponent->SetCapsuleSize(PlayerBasics::PLAYER_RADIUS, this->m_basics.m_currentHeight);

	cameraUpdate();

	this->m_basics.m_movement.resetInputVelocity();
}

void ACB_PlayerCharacter::playerUpdate(float deltaTime)
{
	this->SetActorLocation(this->m_basics.checkPlayerBounds(this->GetActorLocation()));
	this->m_basics.m_movement.setInputRotation(Controller->GetControlRotation().Yaw);

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

	this->m_basics.m_movement.updateVelocity(this->m_basics.m_currentMobility);

	GetCharacterMovement()->Velocity = this->m_basics.m_movement.getMovementVelocity(GetCharacterMovement()->Velocity.Z);
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

	this->grabBox->SetRelativeRotation(playerRotation);
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

	//if (this->DodgeballProjectileClass)
	//{
	//	FActorSpawnParameters spawnParameters;

	//	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//	spawnParameters.bNoFail = true;
	//	spawnParameters.Owner = this;
	//	spawnParameters.Instigator = this;

	//	FTransform spawnTransform;

	//	//Scale forward vector by 20.0f so it won't clip into the capsule collider
	//	FVector spawnLocation = GetActorForwardVector() * 125.0f
	//		+ FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z);

	//	const FRotator& cameraRotation = this->m_basics.m_cameraMovement.getCameraRotation();
	//	const FRotator& playerRotation = this->m_basics.m_cameraMovement.getCameraRotation();

	//	FRotator spawnRotation(Controller->GetControlRotation().Pitch,
	//		GetActorRotation().Yaw + cameraRotation.Yaw - playerRotation.Yaw, 0);

	//	spawnTransform.SetLocation(spawnLocation);
	//	spawnTransform.SetScale3D(FVector(0.5f));

	//	auto dodgeball = GetWorld()->SpawnActor<ACB_DodgeballProjectile>(this->DodgeballProjectileClass,
	//		spawnTransform, spawnParameters);

	//	dodgeball->launchRelease(spawnRotation.RotateVector(Throw::THROW_DIRECTION));
	//}
}

void ACB_PlayerCharacter::StopShootAction()
{
	this->m_throw.onRelease();
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

bool ACB_PlayerCharacter::isGrabbable()
{
	return this->m_basics.getPlayerState() == PlayerBasics::PLAYER_ALIVE;
}

void ACB_PlayerCharacter::makeGrabbed()
{
	this->m_basics.makeGrabbed();
}

void ACB_PlayerCharacter::launchRelease(FVector direction)
{
	this->m_basics.makeAlive();
	this->GetCharacterMovement()->Velocity = direction;// TODO set velocity in direction
}

void ACB_PlayerCharacter::setGrabbedPosition(FVector position)
{
	this->SetActorLocation(position);
	this->GetCharacterMovement()->Velocity = FVector(0.0f, 0.0f, 0.0f);
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