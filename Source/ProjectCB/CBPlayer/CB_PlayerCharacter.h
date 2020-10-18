// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <math.h>
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CB_PlayerCharacter.generated.h"

UCLASS()
class PROJECTCB_API ACB_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACB_PlayerCharacter();

private:

	FVector m_throwDirection = FVector(1, 0, 0.3).GetUnsafeNormal();

	// General

	const float m_walkSpeed = 500.0f;
	const float m_baseGravity = 4.0f;
	const float m_fastGravity = m_baseGravity * 1.5f;
	float m_movementX;
	float m_movementY;
	float m_mobility;

	//bool m_canMove;
	
	// Jump

	//const DodgeMove<(int) this->m_baseGravity * 100> m_dodgeMove;

	const float m_dodgeHeight = 1;
	const float m_dodgeActionMobility = 0.0f;

	const float m_dodgeApexColliderSize = 25.0f;
	const float m_dodgeEndColliderSize = 50.0f;
	const float m_dodgeCooldownColliderSize = 50.f;

	const short m_dodgeFramesToApex = 7; // TODO rename
	const short m_dodgeCooldownFrames = 15;

	// Automatic

	const float m_dodgeVelocity = sqrt(4000 * m_diveHeight * m_baseGravity);

	// dive

	const float m_diveHeight = m_dodgeHeight / 2;
	const float m_diveHorizontalVelocity = 1.75f * m_walkSpeed;
	const float m_diveActionMobility = 0.0f;
	const float m_diveApexColliderSize = 25.0f;
	const float m_diveEndColliderSize = 50.0f;
	const float m_diveCooldownColliderSize = 50.f;

	const short m_diveCooldownFrames = 15;
	const short m_diveFramesToApex = 5;

	float m_diveProportion;

	const float m_diveVerticalVelocity = sqrt(4000 * m_diveHeight * m_baseGravity);

	short m_duckFrame, m_dodgeFrame;

	bool m_dodgeCooldownStarted;

	inline float dodgeProportion(float dodgeValue, float diveValue);

	void dodgeUpdate(UCharacterMovementComponent* characterMovement);

	// Cooldown

	const float m_jumpCooldownColliderSize = 50.f;
	const float m_jumpCooldownMobility = 0.0f;

	const short m_jumpCooldownFrames = 15;

	const float m_leapCooldownColliderSize = 50.f;
	const float m_leapCooldownMobility = 0.0f;

	const short m_leapCooldownFrames = 15;

	// Immutable

	short m_cooldownFrame;

	// duck

	const float m_duckColliderSize = 25.0f; // TODO add in rotation and stuff?

	const short m_duckStartupFrames = 6;
	const short m_duckActionFrames = 24;

	



	// Grab

		// TODO implement

	// Throw

		// TODO implement


	// Size updates // TODO put in general

	float m_previousSize;
	float m_currentSize;
	// end size and frames are dependant on move

	// TODO do the same for rotation

	// Frame counter (for debugging only) TODO remove

	short m_frameCounter;

	bool m_frameCounterActive;


	
	UPROPERTY(EditAnywhere, Category = "Throwing")
	TSubclassOf<class ACB_DodgeballProjectile> DodgeballProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Components")
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Components")
		class USpringArmComponent* cameraArm;

	void adjustGravity(UCharacterMovementComponent* characterMovement);

	float getAnimationPoint(float x);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//For prototyping, TODO add character model and start ik system
	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* staticMesh;
	
	//Add a camera to the character and an arm which will handle clipping through geometry
	//They get setup in the constructor of the cpp file
	

public:

	float BaseTurnRate;
	float BaseLookUpRate;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveVertical(float amount);
	void MoveHorizontal(float amount);
	void LookVertical(float amount);
	void LookHorizontal(float amount);

	void JumpAction();
	void StopJumpAction();

	void RunAction();
	void StopRunAction();

	void ShootAction();
	void StopShootAction();

};
