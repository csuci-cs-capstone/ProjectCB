#include "CB_GridGenerator.h"

const float ACB_GridGenerator::START_SECONDS = 10.0f;
const float ACB_GridGenerator::UPDATE_INTERVAL = 10.0f;

const size_t ACB_GridGenerator::STAGE_WIDTH = 3;
const size_t ACB_GridGenerator::STAGE_LENGTH = 5;

const float ACB_GridGenerator::BOX_SIZE = 300.0f;
const float ACB_GridGenerator::WIDTH_OFFSET = -ACB_GridGenerator::BOX_SIZE * (ACB_GridGenerator::STAGE_WIDTH - 1);
const float ACB_GridGenerator::LENGTH_OFFSET = -ACB_GridGenerator::BOX_SIZE * (ACB_GridGenerator::STAGE_LENGTH - 1);

//ACB_GridBox* ACB_GridGenerator::F2DArray::operator[](size_t pos)
//{
//    return this->m_array[pos];
//}
//
//void ACB_GridGenerator::F2DArray::Add(ACB_GridBox* box)
//{
//    this->m_array.Add(box);
//}

ACB_GridBox* ACB_GridGenerator::spawnBox(size_t lengthPos, size_t widthPos)
{
    FActorSpawnParameters spawnParams;

    FVector location = FVector(ACB_GridGenerator::LENGTH_OFFSET + (2 * lengthPos * ACB_GridGenerator::BOX_SIZE),
        ACB_GridGenerator::WIDTH_OFFSET + (2 * widthPos * ACB_GridGenerator::BOX_SIZE), 0.0f);
    FRotator rotation = FRotator(0.0f, 0.0f, 0.0f);

    return GetWorld()->SpawnActor<ACB_GridBox>(this->BoxClass, location, rotation, spawnParams);
}

void ACB_GridGenerator::generateGrid()
{
    for (size_t length = 0; length < ACB_GridGenerator::STAGE_LENGTH; length++)
    {
        TArray<ACB_GridBox*> boxArray;

        for (size_t width = 0; width < ACB_GridGenerator::STAGE_WIDTH; width++)
        {
            if (length <= (ACB_GridGenerator::STAGE_LENGTH / 2))
            {
                this->m_deletableBoxes.Add((length * ACB_GridGenerator::STAGE_WIDTH) + width);
                this->m_numOfBoxes++;
            }
            
            boxArray.Add(spawnBox(length, width));
        }

        this->m_grid.Add(boxArray);
    }
}

void ACB_GridGenerator::updateGrid()
{
    if (this->m_numOfBoxes > 0)
    {
        this->m_numOfBoxes--;

        size_t boxPos = FMath::RandRange(0, this->m_numOfBoxes);

        size_t boxID = this->m_deletableBoxes[boxPos];

        size_t length = boxID / ACB_GridGenerator::STAGE_WIDTH;
        size_t width = boxID - (length * ACB_GridGenerator::STAGE_WIDTH);

        if (length == (ACB_GridGenerator::STAGE_LENGTH / 2))
            this->m_grid[length][width]->Destroy();
        else
        {
            this->m_grid[ACB_GridGenerator::STAGE_LENGTH - length - 1][ACB_GridGenerator::STAGE_WIDTH - width - 1]->Destroy();
            this->m_grid[length][width]->Destroy();
        }

        this->m_deletableBoxes.RemoveAt(boxPos);
    }
    else
        GetWorldTimerManager().ClearTimer(this->m_timerHandle);
}

ACB_GridGenerator::ACB_GridGenerator()
{
 	PrimaryActorTick.bCanEverTick = false;

    this->m_numOfBoxes = 0;
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

