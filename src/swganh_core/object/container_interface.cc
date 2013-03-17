// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "container_interface.h"
#include "swganh_core/object/permissions/container_permissions_interface.h"
#include "swganh_core/object/object.h"
#include <glm/glm.hpp>
#include <boost/thread/locks.hpp>

using namespace swganh::object;

boost::shared_mutex ContainerInterface::global_container_lock_;

std::shared_ptr<ContainerPermissionsInterface> ContainerInterface::GetPermissions() 
{ 
	return container_permissions_; 
}

void ContainerInterface::SetPermissions(std::shared_ptr<ContainerPermissionsInterface> obj) 
{ 
	container_permissions_ = obj; 
}

bool ContainerInterface::HasContainedObjects()
{
	boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);
	return __InternalHasContainedObjects();
}

bool ContainerInterface::__InternalHasContainedObjects() 
{
	bool has_objects = false;
	std::list<std::shared_ptr<Object>> out;
	__InternalGetObjects(nullptr, 1, true, out);
	return out.size() > 0;
}

std::list<std::shared_ptr<Object>> ContainerInterface::GetObjects(
    const std::shared_ptr<Object>& requester,
    uint32_t max_depth,
    bool topDown)
{
	boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);
	std::list<std::shared_ptr<Object>> out;
	__InternalGetObjects(requester, max_depth, topDown, out);
	return out;
}

void ContainerInterface::GetObjects(
    const std::shared_ptr<Object>& requester,
    uint32_t max_depth,
    bool topDown,
    std::list<std::shared_ptr<Object>>& out)
{
	boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);
	__InternalGetObjects(requester, max_depth, topDown, out);
}

void ContainerInterface::ViewObjects(
    const std::shared_ptr<Object>& requester,
    uint32_t max_depth,
    bool topDown,
    std::function<void (const std::shared_ptr<Object>&)> func)
{
	boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);
	__InternalViewObjects(requester, max_depth, topDown, func);
}

void ContainerInterface::ViewAwareObjects(
    std::function<void (const std::shared_ptr<swganh::object::Object>&)> func,
    const std::shared_ptr<swganh::object::Object>& hint)
{
	boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);
	__InternalViewAwareObjects(func, hint);
}

void ContainerInterface::GetAbsolutes(glm::vec3& pos, glm::quat& rot)
{
	boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);
	__InternalGetAbsolutes(pos, rot);
}
