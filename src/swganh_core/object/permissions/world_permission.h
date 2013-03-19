// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#pragma once

#include "swganh_core/object/permissions/container_permissions_interface.h"

namespace swganh
{
namespace object
{

class WorldPermission : public ContainerPermissionsInterface
{
	virtual PermissionType GetType()
	{
		return WORLD_PERMISSION;
	}

	bool canInsert(std::shared_ptr<Object> container, std::shared_ptr<Object> requester, std::shared_ptr<Object> object);

	bool canRemove(std::shared_ptr<Object> container, std::shared_ptr<Object> requester, std::shared_ptr<Object> object);

	bool canView(std::shared_ptr<Object> container, std::shared_ptr<Object> requester);
};

}
}