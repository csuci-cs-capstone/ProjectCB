// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CB_Dodgeball.generated.h"

UCLASS()
class PROJECTCB_API ACB_Dodgeball : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACB_Dodgeball();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, Category = "Components")
		UStaticMeshComponent* m_mesh;

	const float m_speed = 2.0f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
