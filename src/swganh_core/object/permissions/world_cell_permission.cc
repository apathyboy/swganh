// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "world_cell_permission.h"

#include "swganh_core/object/container_interface.h"

using namespace swganh::object;

bool WorldCellPermission::canInsert(std::shared_ptr<Object> container, std::shared_ptr<Object> requester, std::shared_ptr<Object> object)
{
	return (requester == object);
}

bool WorldCellPermission::canRemove(std::shared_ptr<Object> container, std::shared_ptr<Object> requester, std::shared_ptr<Object> object)
{
	return (requester == object);
}

bool WorldCellPermission::canView(std::shared_ptr<Object> container, std::shared_ptr<Object> requester)
{
	return true;
}