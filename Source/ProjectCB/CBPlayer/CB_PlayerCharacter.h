// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <math.h>
#include "CoreMinimal.h"
#include "PlayerBasics.h"
#include "../CBMoves/Dodge/Dodge.h"
#include "../CBMoves/Throw/Throw.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "CB_PlayerCharacter.generated.h"

UCLASS()
class PROJECTCB_API ACB_PlayerCharacter : public ACharacter, public IGrabbable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties

	PlayerBasics m_basics;

	ACB_PlayerCharacter();

private:

	// General
	
	void playerUpdate(float deltaTime);

	void cameraUpdate();

	void adjustGravity(UCharacterMovementComponent* characterMovement);

	Dodge m_dodge = Dodge(this->m_basics);
	Throw m_throw = Throw(this->m_basics);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		USceneComponent* grabRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
		UBoxComponent* grabBox;

	UFUNCTION()
		void OnEnterGrabBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent,
			int32 otherBodyIndex, bool fromSweep, const FHitResult& sweepResult);

	UFUNCTION()
		void OnLeaveGrabBox(UPrimitiveComponent* overlappedComponent, AActor* otherActor, UPrimitiveComponent* otherComponent,
			int32 otherBodyIndex);

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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveVertical(float amount);
	void MoveHorizontal(float amount);

	void RotateCamera(float amount);

	void JumpAction();
	void StopJumpAction();

	void ShootAction();
	void StopShootAction();

	void AliveAction();

	bool isGrabbable() override;
	void makeGrabbed() override;
	void launchRelease(FVector direction) override;
	void setGrabbedPosition(FVector position) override;
};
