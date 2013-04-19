// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_OBJECT_CONTAINER_INTERFACE_H_
#define SWGANH_OBJECT_CONTAINER_INTERFACE_H_

#include <cstdint>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <vector>

#include <boost/optional.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace swganh {
namespace object {
    
    class ContainerPermissionsInterface;
    class Object;

    typedef std::vector<std::vector<int32_t>> ObjectArrangements;
    
	class ContainerInterface
	{
	public:
        virtual ~ContainerInterface() {}

        virtual uint64_t GetContainmentId() const = 0;

		//Object Management
		virtual void AddObject(
            const std::shared_ptr<Object>& requester,
            std::shared_ptr<Object> object) = 0;

		virtual void RemoveObject(
            const std::shared_ptr<Object>& requester, 
            const std::shared_ptr<Object>& oldObject) = 0;
		
        virtual void TransferObject(
            const std::shared_ptr<Object>& requester,
            const std::shared_ptr<Object>& object,
            const std::shared_ptr<ContainerInterface>& newContainer) = 0;
		
		virtual bool HasContainedObjects() = 0;

		virtual std::list<std::shared_ptr<Object>> GetObjects(
            const std::shared_ptr<Object>& requester, 
            uint32_t max_depth,
            bool topDown) = 0;

		virtual void GetObjects(
            const std::shared_ptr<Object>& requester, 
            uint32_t max_depth, 
            bool topDown, 
            std::list<std::shared_ptr<Object>>& out) = 0;
        
		virtual void ViewObjects(
            const std::shared_ptr<Object>& requester, 
            uint32_t max_depth,
            bool topDown, 
            std::function<void (const std::shared_ptr<Object>&)> func) = 0;

		virtual std::shared_ptr<ContainerPermissionsInterface> GetPermissions() = 0;

		virtual void SetPermissions(std::shared_ptr<ContainerPermissionsInterface> obj) = 0;
        
        virtual void SetContainer(const std::shared_ptr<ContainerInterface>& container) = 0;
        
        virtual const std::shared_ptr<ContainerInterface>& GetContainer() = 0;
        
        template<typename T>
        std::shared_ptr<T> GetContainer()
        {
#ifdef _DEBUG
            return std::dynamic_pointer_cast<T>(GetContainer());
#else
            return std::static_pointer_cast<T>(GetContainer());
#endif
        }

		virtual void GetAbsolutes(glm::vec3& pos, glm::quat& rot) = 0;
	};

}}

#endif
