// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <math.h>
#include "CoreMinimal.h"
#include "PlayerBasics.h"
#include "../CBMoves/Dodge/Dodge_Hold.h"
#include "../CBMoves/Dodge/Dodge_Release.h"
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

	FVector m_throwDirection = FVector(1, 0, 0.05).GetUnsafeNormal();

	// General
	
	PlayerBasics m_basics;

	void cameraUpdate();

	void adjustGravity(UCharacterMovementComponent* characterMovement);

	// Dodge Dip Duck Dive Dodge

	Dodge_Hold m_dodgeHold;
	Dodge_Release m_dodgeRelease;

	void dodgeUpdate(UCharacterMovementComponent* characterMovement);

	// Grab/Catch/Pickup/Throw/Drop

		// TODO implement

// FOR DEBUGGING ONLY (TODO remove)

	short m_frameCounter;
	bool m_frameCounterActive;

	///////////////////////////////////

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//For prototyping, TODO add character model and start ik system
	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* staticMesh;

	UPROPERTY(EditAnywhere, Category = "Throwing")
		TSubclassOf<class ACB_DodgeballProjectile> DodgeballProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Components")
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Components")
		class USpringArmComponent* cameraArm;

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
