#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CB_GridBox.generated.h"

UCLASS()
class PROJECTCB_API ACB_GridBox : public AActor
{
	GENERATED_BODY()

private:

	bool m_isFalling;

public:

	ACB_GridBox();

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* BoxMesh;

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	void makeFall();
};