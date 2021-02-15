#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CB_GridBox.h"
#include "CB_GridGenerator.generated.h"

UCLASS()
class PROJECTCB_API ACB_GridGenerator : public AActor
{
	GENERATED_BODY()

private:

	static const float START_SECONDS;
	static const float UPDATE_INTERVAL;

	FTimerHandle m_timerHandle;

	void spawnBox(size_t gridX, size_t gridY);

	void generateGrid();

	void updateGrid();
	
public:

	ACB_GridGenerator();

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
		TSubclassOf<class ACB_GridBox> BoxClass;

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

};
