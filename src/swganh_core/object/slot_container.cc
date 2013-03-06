// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "slot_container.h"
#include "object.h"

using namespace swganh::object;

std::shared_ptr<Object> SlotContainer::insert_object(std::shared_ptr<Object> insertObject)
{
    held_objects_.emplace(insertObject);
	return nullptr;
}

void SlotContainer::remove_object(const std::shared_ptr<Object>& removeObject)
{
    held_objects_.erase(removeObject);
}

void SlotContainer::view_objects(ViewWalkerFunction walkerFunction)
{
	for_each(held_objects_.begin(), held_objects_.end(), walkerFunction);
}
