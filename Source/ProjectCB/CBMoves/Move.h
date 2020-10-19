// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECTCB_API Move
{
	// Warmup -> Hold, Cooldown -> Release
	// Start (set frames)
	// End (end function)

public:
	// Start

	float m_colliderRadius;
	float m_colliderHeight;

	FVector m_position, m_velocity, m_accelertation;

	FRotator m_colliderRotation;

	// End

	//bool shouldEnd();
};
