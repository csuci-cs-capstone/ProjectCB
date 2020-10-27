#include "Proportion.h"

Proportion::Proportion() {}

Proportion::Proportion(const float proportion) : m_proportion(proportion) {}

float Proportion::getProportion(const float propVal, const float invVal)
{
	return (this->m_proportion * propVal) + (this->invProp() * invVal);
}

float Proportion::invProp()
{
	return 1 - this->m_proportion;
}
