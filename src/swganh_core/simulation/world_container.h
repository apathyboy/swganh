// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include "swganh_core/object/container_interface.h"
#include "swganh_core/object/permissions/world_permission.h"

namespace swganh {
namespace simulation {

class SpatialProviderInterface;

/**
 * @brief
 */
class WorldContainer : public swganh::object::ContainerInterface
{
public:
	WorldContainer(SpatialProviderInterface* spatial_provider)
		: spatial_provider_(spatial_provider)
	{ SetPermissions(std::shared_ptr<swganh::object::ContainerPermissionsInterface>(new swganh::object::WorldPermission())); }
	~WorldContainer() { };

	uint64_t GetObjectId() { return 0; }

	void AddObject(std::shared_ptr<swganh::object::Object> requester, std::shared_ptr<swganh::object::Object> newObject, int32_t arrangement_id=-2);
	void RemoveObject(std::shared_ptr<swganh::object::Object> requester, std::shared_ptr<swganh::object::Object> oldObject);
	void TransferObject(std::shared_ptr<swganh::object::Object> requester, std::shared_ptr<swganh::object::Object> object, std::shared_ptr<ContainerInterface> newContainer, glm::vec3 position, int32_t arrangement_id=-2);
	
	// 
	void __InternalGetObjects(std::shared_ptr<swganh::object::Object> requester, uint32_t max_depth, bool topDown, std::list<std::shared_ptr<swganh::object::Object>>& out) { std::cout << "WorldContainer::__InternalGetObjects" << std::endl; }
	void __InternalViewObjects(std::shared_ptr<swganh::object::Object> requester, uint32_t max_depth, bool topDown, std::function<void(std::shared_ptr<swganh::object::Object>)> func) { std::cout << "WorldContainer::__InternalViewObjects" << std::endl; }
	int32_t __InternalInsert(std::shared_ptr<swganh::object::Object> object, glm::vec3 new_position, int32_t arrangement_id=-2);
	void __InternalViewAwareObjects(std::function<void(std::shared_ptr<swganh::object::Object>)> func, std::shared_ptr<swganh::object::Object> hint=nullptr) { std::cout << "WorldContainer::__internalViewAwareObjects" << std::endl; }
	std::shared_ptr<ContainerInterface> GetContainer() { return nullptr; }
	void SetContainer(const std::shared_ptr<ContainerInterface>& container) { std::cout << "WorldContainer::SetContainer" << std::endl; }
	void __InternalGetAbsolutes(glm::vec3& pos, glm::quat& rot) { pos = glm::vec3(); rot = glm::quat(); }

private:
	SpatialProviderInterface* spatial_provider_;
};

}} // namespace swganh::simulation