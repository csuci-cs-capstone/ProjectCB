#pragma once

#include "CoreMinimal.h"

struct PROJECTCB_API Proportion
{
	float m_proportion;

	Proportion();

	Proportion(const float proportion);

	float getProportion(const float propVal, const float invVal);

	float invProp();
};
