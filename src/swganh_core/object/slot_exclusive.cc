// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "slot_exclusive.h"
#include "object.h"

using namespace swganh::object;

std::shared_ptr<Object> SlotExclusive::insert_object(std::shared_ptr<Object> insertObject)
{
    std::swap(held_object_, insertObject);

	return insertObject;
}

void SlotExclusive::remove_object(const std::shared_ptr<Object>& removeObject)
{
	if(held_object_ == removeObject) 
	{
		held_object_ = nullptr;
	}
}

void SlotExclusive::view_objects(ViewWalkerFunction walkerFunction)
{
	if(held_object_ != nullptr)
    {
		walkerFunction(held_object_);
    }
}
