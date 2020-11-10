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

		if (grabbable->getGrabbableObject()->isGrabbable())
			return grabbable;
	}

	return nullptr;
}

void GrabbableList::add(IGrabbable* grabbable)
{
	IGrabbableObject* grabbableObject = grabbable ? grabbable->getGrabbableObject() : nullptr;

	if (grabbableObject)
	{
		unsigned char priority = grabbable->getGrabPriority();

		int pos = 0;

		for (; pos < this->m_length; pos++)
		{
			int nextPos = pos + 1;

			if (grabbable == this->m_grabbableList[pos])
				return;
			else if (priority < this->m_grabbableList[pos]->getGrabPriority())
				break;
		}

		int lastPos = this->m_length;

		while (lastPos > pos)
		{
			int priorPos = lastPos - 1;

			this->m_grabbableList[lastPos] = this->m_grabbableList[priorPos];

			lastPos = priorPos;
		}

		this->m_grabbableList[lastPos] = grabbable;

		this->incrList();
	}
}

void GrabbableList::remove(IGrabbable* grabbable)
{
	if (grabbable)
	{
		int pos = 0;

		for (; pos < this->m_length; pos++)
			if (grabbable == this->m_grabbableList[pos])
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