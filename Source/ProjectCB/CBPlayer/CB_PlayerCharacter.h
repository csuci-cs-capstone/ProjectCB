#pragma once

#include <math.h>
#include "CoreMinimal.h"
#include "PlayerBasics.h"
#include "../CBMoves/Dodge/Dodge.h"
#include "../CBMoves/Throw/Throw.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Animation/BlendSpace1D.h"
#include "CB_PlayerCharacter.generated.h"

class UMaterialInstanceDynamic;

UCLASS()
class PROJECTCB_API ACB_PlayerCharacter : public ACharacter, public IGrabbable, public IGrabbableObject
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	
	UPROPERTY(Replicated)
	FPlayerBasics m_basics;

	ACB_PlayerCharacter();

	//Art Model Anim Extra
	UPROPERTY(Replicated)
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(Replicated)
	UMaterialInstanceDynamic* DynamicHeadMaterial;

	//Networked Anim Properties
	UPROPERTY(Replicated)
	bool bIsOnGroundAnimate;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "PlayerValues")
	bool bIsMultiplayer = true;

	UPROPERTY(Replicated)
	bool bIsGhost = false;

private:

	// General
	short m_resetCollisionFrame;
	
	void playerUpdate(float deltaTime);

	void cameraUpdate();

	void adjustGravity(UCharacterMovementComponent* characterMovement);

	Dodge m_dodge = Dodge(this->m_basics);
	UPROPERTY(Replicated)
		UThrow* m_throw;

	UPROPERTY(Replicated)
		FVector MoveVelocity;

	UPROPERTY(Replicated)
		FTransform StartTransform;

	// Network Replication Player State
	virtual void OnRep_PlayerState() override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, EditAnywhere)
		USceneComponent* grabRoot;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Components")
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

	UPROPERTY(EditAnywhere, Category = "Components")
		USkeletalMeshComponent* skeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Throwing")
		TSubclassOf<class ACB_DodgeballProjectile> DodgeballProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Components")
		class UCameraComponent* camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Components")
		class USpringArmComponent* cameraArm;

	//ANIM
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Anims")
		UBlendSpace1D* blendspace;

	USkeletalMesh* m_PlayerGhostModel;

	//Networking
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	UFUNCTION(BlueprintCallable,Category="Options")
		void updateCameraSensitivity(float cameraSensitivity);

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

	//Networked Moves
	//TESTING RPC FOR VELOCITY
	//TODO Make countdown and check head band to orange, set up UI values and players status
	UFUNCTION(Server, Reliable)
	void UpdateVelocity(FVector newVelocityVector);

	UFUNCTION(NetMulticast, Reliable)
	void KnockBackPlayer(FVector HitNormal);

	UFUNCTION(Server, Reliable)
	void CheckIfPlayerIsAlive();

	UFUNCTION(NetMulticast, Reliable)
	void MakePlayerIntoGhost();

	UFUNCTION(NetMulticast, Reliable)
	void MakePlayerAlive();

	UFUNCTION(Server, Reliable)
	void SetPlayerStartPosition(FVector incomingPosition, FRotator incomingRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void SendLocalClientRotationToServer();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SetPlayerMaterialColor();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SetPlayerStartRotation();

	UFUNCTION(Server, Reliable)
	void LaunchBall();

	UFUNCTION(Server, Reliable)
	void RemoveBall(ACB_DodgeballProjectile* currentBall);

	UFUNCTION(Server, Reliable)
	void UpdateGrabbedObjectPosition(UObject* currentGrabbedObject);

	float getRadius() override;
	bool isGrabbable() override;
	void makeGrabbed() override;
	void launchRelease(FVector direction, FRotator rotation) override;
	void setGrabbed(FVector position, FRotator rotation) override;

	bool hasGrabbableObject() override;
	IGrabbableObject* getGrabbableObject() override;
	unsigned char getGrabPriority() override;
	/// @brief Adds an actor to the ignore collisions list
	/// @param spawnedActor 
	void ignoreCollisionsOnThrownObject(AActor* spawnedActor);

	//Animation Updating Functions
	bool onGround();
	bool onDuck();
	bool onThrowing();
	void resetThrowing();
	/// @brief Checks X and Y speed only, used for animations
	float groundMoveSpeed();
	/// @brief Check if the PlayerCharacter has a Grabbable Object, used for the catch animation
	bool onCatch();

};
