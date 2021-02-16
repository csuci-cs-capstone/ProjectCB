#include "CB_GridBox.h"

ACB_GridBox::ACB_GridBox()
{
 	PrimaryActorTick.bCanEverTick = false;

	this->m_isFalling = false;

	this->BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>("BoxMesh");

	SetRootComponent(this->BoxMesh);
}

void ACB_GridBox::BeginPlay()
{
	Super::BeginPlay();
}

void ACB_GridBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (this->m_isFalling)
	//{
	//	// update position
	//}
}

