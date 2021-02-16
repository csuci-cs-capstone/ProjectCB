#include "CB_GridGenerator.h"

const float ACB_GridGenerator::START_SECONDS = 10.0f;
const float ACB_GridGenerator::UPDATE_INTERVAL = 10.0f;

const size_t ACB_GridGenerator::STAGE_WIDTH = 3;
const size_t ACB_GridGenerator::STAGE_LENGTH = 5;

const float ACB_GridGenerator::BOX_SIZE = 300.0f;
const float ACB_GridGenerator::WIDTH_OFFSET = -ACB_GridGenerator::BOX_SIZE * (ACB_GridGenerator::STAGE_WIDTH - 1);
const float ACB_GridGenerator::LENGTH_OFFSET = -ACB_GridGenerator::BOX_SIZE * (ACB_GridGenerator::STAGE_LENGTH - 1);

void ACB_GridGenerator::spawnBox(size_t lengthPos, size_t widthPos)
{
    FActorSpawnParameters spawnParams;

    FVector location = FVector(ACB_GridGenerator::LENGTH_OFFSET + (2 * lengthPos * ACB_GridGenerator::BOX_SIZE),
        ACB_GridGenerator::WIDTH_OFFSET + (2 * widthPos * ACB_GridGenerator::BOX_SIZE), 0.0f);
    FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);

    ACB_GridBox* spawnedBox = GetWorld()->SpawnActor<ACB_GridBox>(this->BoxClass, location, rotation, spawnParams);
}

void ACB_GridGenerator::generateGrid()
{
    for (size_t length = 0; length < ACB_GridGenerator::STAGE_LENGTH; length++)
        for (size_t width = 0; width < ACB_GridGenerator::STAGE_WIDTH; width++)
            spawnBox(length, width);
}

void ACB_GridGenerator::updateGrid()
{
   


    //GetWorldTimerManager().ClearTimer(this->m_timerHandle);
}

ACB_GridGenerator::ACB_GridGenerator()
{
 	PrimaryActorTick.bCanEverTick = false;
}

void ACB_GridGenerator::BeginPlay()
{
    Super::BeginPlay();

    generateGrid();

    GetWorldTimerManager().SetTimer(this->m_timerHandle, this, &ACB_GridGenerator::updateGrid, ACB_GridGenerator::START_SECONDS, true,
        ACB_GridGenerator::UPDATE_INTERVAL);
}

void ACB_GridGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

