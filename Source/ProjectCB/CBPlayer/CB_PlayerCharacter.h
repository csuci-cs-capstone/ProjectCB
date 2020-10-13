// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CB_PlayerCharacter.generated.h"
//#include "../CBObjects/CB_Dodgeball.h"

UCLASS()
class PROJECTCB_API ACB_PlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACB_PlayerCharacter();

private:

	//// Dodgeball class to spawn.
	//UPROPERTY(EditDefaultsOnly, Category = Projectile)
	//	TSubclassOf<class ACB_Dodgeball> DodgeballClass;

	//FVector m_dodgeballOffset;

	const float m_walkSpeed = 500.0f;
	const float m_baseGravity = 4.0f;
	const float m_fastGravity = m_baseGravity * 1.5f;
	const float m_jumpVelocity = 1200;
	const float m_baseAirControl = 0.75f;

	float xMovement;
	float yMovement;

	bool m_startedLeap;
	bool m_inLeap;
	

	//UPROPERTY(EditAnywhere, Category("Throwing"))
	//class TSubclassOf<class ACB_Dodgeball> DodgeballClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//For prototyping, TODO add character model and start ik system
	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* staticMesh;
	
	//Add a camera to the character and an arm which will handle clipping through geometry
	//They get setup in the constructor of the cpp file
	UPROPERTY(EditAnywhere, Category = "Components")
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere, Category = "Components")
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
