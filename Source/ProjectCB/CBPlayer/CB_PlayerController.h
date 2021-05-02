#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "CB_PlayerController.generated.h"


UCLASS()
class PROJECTCB_API ACB_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ACB_PlayerController();

	virtual void BeginPlay();

	void Tick(float DeltaSeconds);

	virtual void SetupInputComponent();

	void MoveVertical(float amount);
	void MoveHorizontal(float amount);

	void RotateCamera(float amount);

	void JumpAction();
	void StopJumpAction();

	//UFUNCTION(Client, Reliable, WithValidation)
	void ShootAction();
	//UFUNCTION(Client, Reliable, WithValidation)
	void StopShootAction();
	
	void AliveAction();
	
	//For Gamemode use
	bool m_bIsPlayerControlEnabled = false;

	UFUNCTION(NetMulticast, Reliable)
	void SetPlayerControlEnabled(bool isEnabled);


};
