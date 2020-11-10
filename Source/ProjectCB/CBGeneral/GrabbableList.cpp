#include "GrabbableList.h"

void GrabbableList::initializeList(const size_t size)
{
	this->m_size = size;
	this->m_grabbableList = (IGrabbable**) malloc(sizeof(IGrabbable*) * this->m_size);
}

void GrabbableList::resizeList()
{
	IGrabbable** oldList = this->m_grabbableList;

	this->initializeList(this->m_size * 2);

	for (int pos = 0; pos < this->m_length; pos++)
		this->m_grabbableList[pos] = oldList[pos];

	free(oldList);
}

void GrabbableList::incrList()
{
	this->m_length++;

	if (this->m_length >= this->m_size)
		this->resizeList();
}

void GrabbableList::decrList()
{
	this->m_length--;
}

GrabbableList::GrabbableList()
{
	this->initializeList(4);

	this->m_length = 0;
}

GrabbableList::~GrabbableList()
{
	free(this->m_grabbableList);
}

IGrabbable* GrabbableList::getTopGrabbable()
{
	for (int pos = this->m_length - 1; pos >= 0; pos--)
	{
		IGrabbable* grabbable = this->m_grabbableList[pos];

		if (grabbable->isGrabbable())
			return grabbable;
	}

	return nullptr;
}

void GrabbableList::add(IGrabbable* grabbableObject)
{
	if (grabbableObject && grabbableObject->isGrabbable())
	{
		unsigned char priority = grabbableObject->getGrabPriority();

		int pos = this->m_length;

		while (pos > 0)
		{
			int priorPos = pos - 1;

			if (priority >= this->m_grabbableList[priorPos]->getGrabPriority())
				break;

			pos = priorPos;
		}

		this->m_grabbableList[pos] = grabbableObject;

		this->incrList();
	}
}

void GrabbableList::remove(IGrabbable* grabbableObject)
{
	if (grabbableObject)
	{
		int pos = 0;

		for (; pos < this->m_length; pos++)
			if (grabbableObject == this->m_grabbableList[pos])
				break;

		int nextPos = pos + 1;

		for (; nextPos < this->m_length; nextPos++)
		{
			this->m_grabbableList[pos] = this->m_grabbableList[nextPos];

			pos = nextPos;
		}

		if (pos < this->m_length)
		{
			this->m_grabbableList[pos] = nullptr;

			this->decrList();
		}
	}
}