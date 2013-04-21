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
        , null_container_(nullptr)
	{ SetPermissions(std::shared_ptr<swganh::object::ContainerPermissionsInterface>(new swganh::object::WorldPermission())); }
	~WorldContainer() { };

    uint64_t GetContainmentId() const { return 0; }

    virtual void AddObject(
        const std::shared_ptr<swganh::object::Object>& requester,
        std::shared_ptr<swganh::object::Object> object);
    
    virtual void RemoveObject(
        const std::shared_ptr<swganh::object::Object>& requester, 
        const std::shared_ptr<swganh::object::Object>& oldObject);
    
    virtual void TransferObject(
        const std::shared_ptr<swganh::object::Object>& requester,
        const std::shared_ptr<swganh::object::Object>& object,
        const std::shared_ptr<swganh::object::ContainerInterface>& newContainer);
    
    virtual bool HasContainedObjects() { return true; }
    
    virtual std::list<std::shared_ptr<swganh::object::Object>> GetObjects(
        const std::shared_ptr<swganh::object::Object>& requester, 
        uint32_t max_depth,
        bool topDown)
    { 
        std::cout << "WorldContainer::GetObjects" << std::endl; 
        return std::list<std::shared_ptr<swganh::object::Object>>();
    }
    
    virtual void GetObjects(
        const std::shared_ptr<swganh::object::Object>& requester, 
        uint32_t max_depth, 
        bool topDown, 
        std::list<std::shared_ptr<swganh::object::Object>>& out)
    { 
        std::cout << "WorldContainer::GetObjects" << std::endl;
    }
    
    virtual void ViewObjects(
        const std::shared_ptr<swganh::object::Object>& requester, 
        uint32_t max_depth,
        bool topDown, 
        std::function<void (const std::shared_ptr<swganh::object::Object>&)> func)
    { 
        std::cout << "WorldContainer::ViewObjects" << std::endl;
    }
    
    virtual std::shared_ptr<swganh::object::ContainerPermissionsInterface> GetPermissions() { return nullptr; }
    
    virtual void SetPermissions(std::shared_ptr<swganh::object::ContainerPermissionsInterface> obj) {}

    std::shared_ptr<swganh::object::ContainerInterface> GetContainer() { return null_container_; }
	void SetContainer(const std::shared_ptr<swganh::object::ContainerInterface>& container) { std::cout << "WorldContainer::SetContainer" << std::endl; }

	virtual void GetAbsolutes(glm::vec3& pos, glm::quat& rot) { pos = glm::vec3(); rot = glm::quat(); }

private:
	SpatialProviderInterface* spatial_provider_;
    std::shared_ptr<swganh::object::ContainerInterface> null_container_;

};

}} // namespace swganh::simulation