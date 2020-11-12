#pragma once

#include "CoreMinimal.h"
#include "Grabbable.h"

class PROJECTCB_API GrabbableList
{
private:

	IGrabbable** m_grabbableList;
	size_t m_size; // memory allocated
	size_t m_length; // elements in array

	void initializeList(const size_t size);

	void resizeList();

	void incrList();

	void decrList();

public:

	GrabbableList();

	~GrabbableList();

	IGrabbable* getTopGrabbable();

	void add(IGrabbable* grabbable);

	void remove(IGrabbable* grabbable);

	size_t length();
};
