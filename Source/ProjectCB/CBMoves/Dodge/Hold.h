// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECTCB_API Hold
{
public:
	// Mutable
	const float m_colliderSize = 25.0f;

	const short m_startupFrames = 6;
	const short m_actionFrames = 24;

	// Immutable

	short m_frame;
};
