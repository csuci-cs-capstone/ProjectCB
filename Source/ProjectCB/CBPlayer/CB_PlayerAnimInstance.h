// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CB_PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTCB_API UCB_PlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UCB_PlayerAnimInstance();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsJumping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsDucking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsThrowing;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float moveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bCurrentlyGrabbing;
	
	UFUNCTION(BlueprintCallable, Category = "UpdatePlayerAnimProperties")
		void UpdateAnimationProperties();

	UFUNCTION(BlueprintCallable, Category = "UpdateThrowAnimation")
		void UpdateThrowAnimation();
};
