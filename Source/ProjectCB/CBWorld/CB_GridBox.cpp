#include "CB_GridBox.h"
#include "Net/UnrealNetwork.h"

ACB_GridBox::ACB_GridBox()
{
 	PrimaryActorTick.bCanEverTick = false;
	this->SetReplicates(true);

	this->BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>("BoxMesh");
	SetRootComponent(this->BoxMesh);

	//this->StandardMaterial = CreateDefaultSubobject<UMaterial>(TEXT("StandardMaterial"));
	//this->FallingMaterial = CreateDefaultSubobject<UMaterial>(TEXT("FallingMaterial"));

	//this->BoxMesh->SetMaterial(0, this->StandardMaterial);
}

void ACB_GridBox::BeginPlay()
{
	Super::BeginPlay();
}

void ACB_GridBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACB_GridBox::startFall()
{
	//this->BoxMesh->SetMaterial(0, this->FallingMaterial);
}