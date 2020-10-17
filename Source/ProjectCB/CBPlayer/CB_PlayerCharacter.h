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

	bool m_canMove;
	
	// Jump

	

	// Dodge Dip Duck Dive Dodge

	// Dodge (Move)

		// Duck (Hold)

	//const float m_duckHeight;

	const short m_duckStartupFrames = 15;

	bool m_ducked;

		// Dodge (Release)

	const float m_dodgeHeight = 1;
	const float m_dodgeControl = 0.0f;

	const short m_dodgeCooldownFrames = 15;

	bool m_dodged;

		// Dive (Release + Direction)

	const float m_diveHeight = m_dodgeHeight / 2;
	const float m_diveHorizontalVelocity = 1.75f * m_walkSpeed;
	const float m_diveControl = 0.0f;

	const short m_diveCooldownFrames = 15;

	float m_diveAmount;

	bool m_dived; // TODO remove

		// Dodge (Move)

	const float m_dodgeVelocity = sqrt(400000 * m_dodgeHeight * m_baseGravity);
	const float m_diveVerticalVelocity = sqrt(400000 * m_diveHeight * m_baseGravity);

	short m_duckFrame;
	short m_dodgeFrame;

	bool m_dodgeCooldownStarted;

	void dodgeUpdate(UCharacterMovementComponent* characterMovement);

	// Grab

		// TODO implement

	// Throw

		// TODO implement

	

	
	UPROPERTY(EditAnywhere, Category = "Throwing")
	TSubclassOf<class ACB_DodgeballProjectile> DodgeballProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Components")
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Components")
		class USpringArmComponent* cameraArm;

	void adjustGravity(UCharacterMovementComponent* characterMovement);

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
