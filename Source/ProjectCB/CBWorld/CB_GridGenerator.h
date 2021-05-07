#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CB_GridBox.h"
#include "../CBObjects/CB_DodgeballProjectile.h"
#include "Math/UnrealMathUtility.h"
#include "CB_GridGenerator.generated.h"

UCLASS()
class PROJECTCB_API ACB_GridGenerator : public AActor
{
	GENERATED_BODY()

private:

	static const float START_SECONDS;
	static const float UPDATE_INTERVAL;
	static const float FALL_TIME;

	static const int STAGE_WIDTH;
	static const int STAGE_LENGTH;

	static const float BOX_SIZE;
	static const float WIDTH_OFFSET;
	static const float LENGTH_OFFSET;

	static const float BALL_SPAWN_START_SECONDS;
	static const float BALL_SPAWN_UPDATE_INTERVAL;

	int m_CurrentState = 0;

	TArray<size_t> m_deletableBoxes;
	int m_numOfBoxes;
	TArray<TArray<ACB_GridBox*>> m_grid;

	int m_numOfFallingBoxes;
	UPROPERTY(Replicated)
	TArray<ACB_GridBox*> m_fallingBoxes;
	UPROPERTY(Replicated)
	FTimerHandle m_timerHandle;
	UPROPERTY(Replicated)
	FTimerHandle m_fallHandle;
	UPROPERTY(Replicated)
	FTimerHandle m_ballHandle;

	ACB_GridBox* spawnBox(int lengthPos, int widthPos);
	UFUNCTION(Server, Reliable)
		void spawnBall(int lengthPos, int widthPos);
	UFUNCTION(Server, Reliable)
		void spawnBalls();
	UFUNCTION(Server, Reliable)
	void deleteBoxes();
	UFUNCTION(Server, Reliable)
	void generateGrid();
	UFUNCTION(Server, Reliable)
	void updateGrid();
	
public:

	ACB_GridGenerator();

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		TSubclassOf<class ACB_GridBox> BoxClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		TSubclassOf<class ACB_DodgeballProjectile> DodgeballClass;

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;
	UFUNCTION(Server, Reliable)
		void EnableStageRemoval();

	UFUNCTION(Server, Reliable)
		void DisableStageRemoval();

	UFUNCTION(Server, Reliable)
		void DisableBallGeneration();

};
