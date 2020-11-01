#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CB_PlayerController.generated.h"

UCLASS()
class PROJECTCB_API ACB_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ACB_PlayerController();

	virtual void BeginPlay();

	virtual void SetupInputComponent();

	void MoveVertical(float amount);
	void MoveHorizontal(float amount);

	void RotateCamera(float amount);

	void JumpAction();
	void StopJumpAction();

	void ShootAction();
	void StopShootAction();
	
};
