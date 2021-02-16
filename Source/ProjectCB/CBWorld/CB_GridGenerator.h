#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CB_GridBox.h"
#include "Math/UnrealMathUtility.h"
#include "CB_GridGenerator.generated.h"

UCLASS()
class PROJECTCB_API ACB_GridGenerator : public AActor
{
	GENERATED_BODY()

private:

	//USTRUCT()
	//struct F2DArray
	//{
	//	GENERATED_BODY()

	//public:

	//	TArray<ACB_GridBox*> m_array;

	//	ACB_GridBox* operator[] (size_t pos);

	//	void Add(ACB_GridBox* box);
	//};

	static const float START_SECONDS;
	static const float UPDATE_INTERVAL;

	static const size_t STAGE_WIDTH;
	static const size_t STAGE_LENGTH;

	static const float BOX_SIZE;
	static const float WIDTH_OFFSET;
	static const float LENGTH_OFFSET;

	TArray<size_t> m_deletableBoxes;
	size_t m_numOfBoxes;
	TArray<TArray<ACB_GridBox*>> m_grid;
	FTimerHandle m_timerHandle;

	ACB_GridBox* spawnBox(size_t lengthPos, size_t widthPos);

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
