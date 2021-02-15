#include "CB_GridGenerator.h"

const float ACB_GridGenerator::START_SECONDS = 10.0f;
const float ACB_GridGenerator::UPDATE_INTERVAL = 10.0f;

void ACB_GridGenerator::spawnBox(size_t gridX, size_t gridY)
{
    FActorSpawnParameters spawnParams;

    FVector location = FVector(0.0f, 0.0f, 500.0f);
    FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);

    ACB_GridBox* spawnedBox = GetWorld()->SpawnActor<ACB_GridBox>(this->BoxClass, location, rotation, spawnParams);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("spawned box")));
}

void ACB_GridGenerator::generateGrid()
{
    
}

void ACB_GridGenerator::updateGrid()
{
    // Once we've called this function enough times, clear the Timer.
    //if (--RepeatingCallsRemaining <= 0)
    //{
    //    GetWorldTimerManager().ClearTimer(MemberTimerHandle);
    //    // MemberTimerHandle can now be reused for any other Timer.
    //}
    // Do something here...
}

ACB_GridGenerator::ACB_GridGenerator()
{
 	PrimaryActorTick.bCanEverTick = false;
}

void ACB_GridGenerator::BeginPlay()
{
    Super::BeginPlay();

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("begun play")));

    spawnBox(0, 0);

    generateGrid();

    GetWorldTimerManager().SetTimer(this->m_timerHandle, this, &ACB_GridGenerator::updateGrid, ACB_GridGenerator::START_SECONDS, true,
        ACB_GridGenerator::UPDATE_INTERVAL);
}

void ACB_GridGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

