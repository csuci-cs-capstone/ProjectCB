// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct PROJECTCB_API Hold
{
	// Warmup -> Hold, Cooldown -> Release
	// Start (set frames)
		// collision
	// End (end function)

	// Mutable
	const float m_colliderSize = 25.0f;

	const float m_startupMobility = 0.0f;
	const float m_actionMobility = 0.0f;

	const short m_startupFrames = 6;
	const short m_actionFrames = 24;

	// Immutable

	short m_frame;
};
