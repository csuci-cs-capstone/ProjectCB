// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <math.h>
#include "CoreMinimal.h"
#include "PlayerBasics.h"
#include "../CBMoves/Dodge/Hold.h"
#include "../CBMoves/Dodge/Release.h"
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
	
	//PlayerBasics m_basics; // TODO rename to PlayerConstants ?

	float m_currentWorldLocationZ = g_playerStartWorldLocationZ;
	float m_worldLocationProportionZ = 0.75f;

	float m_movementX;
	float m_movementY;
	float m_mobility;

	float m_currentSize = 50.0f;
	float m_previousSize;

	
	float getAnimationPoint(float x);
	
	// Jump

	

	// Dodge Dip Duck Dive Dodge

	// Dodge (Move)

		// Duck (Hold)

	//const float m_duckHeight;

	Hold m_duck;
	Release m_dodge;

		// Dodge (Move)

	

	void dodgeUpdate(UCharacterMovementComponent* characterMovement);

	inline float dodgeProportion(float dodgeValue, float diveValue);


	// Grab

		// TODO implement

	// Throw

		// TODO implement


	// Size updates // TODO put in general

	
	// end size and frames are dependant on move

	// TODO do the same for rotation

// FOR DEBUGGING ONLY (TODO remove)

	short m_frameCounter;
	bool m_frameCounterActive;

///////////////////////////////////

	
	UPROPERTY(EditAnywhere, Category = "Throwing")
	TSubclassOf<class ACB_DodgeballProjectile> DodgeballProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Components")
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"),Category = "Components")
		class USpringArmComponent* cameraArm;

	void cameraUpdate();

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
