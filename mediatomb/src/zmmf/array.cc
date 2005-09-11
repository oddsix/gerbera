/*  array.cc - this file is part of MediaTomb.
                                                                                
    Copyright (C) 2005 Gena Batyan <bgeradz@deadlock.dhs.org>,
                       Sergey Bostandzhyan <jin@deadlock.dhs.org>
                                                                                
    MediaTomb is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
                                                                                
    MediaTomb is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                                                                                
    You should have received a copy of the GNU General Public License
    along with MediaTomb; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "array.h"

#include <string.h>
#include "memory.h"

using namespace zmm;

ArrayBase::ArrayBase()
{}

void ArrayBase::init(int capacity)
{
	this->capacity = capacity;
	siz = 0;
	arr = (Object **)MALLOC(capacity * sizeof(Object *));
}
ArrayBase::~ArrayBase()
{
	for(int i = 0; i < siz; i++)
	{
		Object *obj = arr[i];
		if(obj)
			obj->release();
	}
    FREE(arr);
}

void ArrayBase::append(Object *obj)
{
	obj->retain();
	resize(siz + 1);
	arr[siz++] = obj;
}

void ArrayBase::set(Object *obj, int index)
{
	Object *old = arr[index];
	if(old)
		old->release();
	if(obj)
		obj->retain();
	arr[index] = obj;
}
Object *ArrayBase::get(int index)
{
	return arr[index];
}
void ArrayBase::remove(int index, int count)
{
    if (index >= siz) // index beyond size
        return;
	int max = index + count; // max is the last element to remove + 1
    if (max > siz) // if remove block is beyond size, cut it
        max = siz;
    if (max <= index) // if nothing to remove
        return;
	for(int i = index; i < max; i++)
	{
		Object *obj = arr[i];
		if(obj)
			obj->release();
	}
    int move = siz - max;
    if (move) // if there is anything to shift
    {
    	memmove(
	    	(void *)(arr + index),
		    (void *)(arr + index + count),
    		move * sizeof(Object *)
	    );
    }
	siz -= count;
}
void ArrayBase::insert(int index, Object *obj)
{
	resize(siz + 1);
	memmove(
		(void *)(arr + (index + 1)),
		(void *)(arr + index),
		(siz - index) * sizeof(Object *)
	);
	obj->retain();
	arr[index] = obj;
    siz++;
}

void ArrayBase::optimize()
{
    capacity = siz;
    arr = (Object **)REALLOC(arr, capacity * sizeof(Object *));
}

void ArrayBase::resize(int requiredSize)
{
	if(requiredSize > capacity)
	{
		int newCapacity = siz + (siz / 2);
		if(requiredSize > newCapacity)
			newCapacity = requiredSize;
		capacity = newCapacity;
		arr = (Object **)REALLOC(arr, capacity * sizeof(Object *));
	}
}
