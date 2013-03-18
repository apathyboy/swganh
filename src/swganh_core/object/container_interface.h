// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#pragma once

#include <list>
#include <memory>
#include <functional>

#include <swganh/observer/observer_interface.h>
#include <boost/thread/shared_mutex.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace swganh
{
namespace object
{
	class Object;
	class ContainerPermissionsInterface;

	class ContainerInterface
	{
	public:
		
		virtual uint64_t GetObjectId() = 0;

		//Object Management
		virtual void AddObject(
            const std::shared_ptr<Object>& requester,
            std::shared_ptr<Object> newObject,
            int32_t arrangement_id=-2) = 0;

		virtual void RemoveObject(
            const std::shared_ptr<Object>& requester, 
            const std::shared_ptr<Object>& oldObject) = 0;
		
        virtual void TransferObject(
            const std::shared_ptr<Object>& requester,
            const std::shared_ptr<Object>& object,
            const std::shared_ptr<Object>& newContainer, 
            glm::vec3 position, 
            int32_t arrangement_id=-2) = 0;
		
        virtual void SwapSlots(
            const std::shared_ptr<Object>& requester,
            const std::shared_ptr<Object>& object,
            int32_t new_arrangement_id) {};

		bool HasContainedObjects();

		std::list<std::shared_ptr<Object>> GetObjects(
            const std::shared_ptr<Object>& requester, 
            uint32_t max_depth,
            bool topDown);

		void GetObjects(
            const std::shared_ptr<Object>& requester, 
            uint32_t max_depth, 
            bool topDown, 
            std::list<std::shared_ptr<Object>>& out);
        
		void ViewObjects(
            const std::shared_ptr<Object>& requester, 
            uint32_t max_depth,
            bool topDown, 
            std::function<void (const std::shared_ptr<Object>&)> func);

		virtual std::shared_ptr<ContainerPermissionsInterface> GetPermissions();
		virtual void SetPermissions(std::shared_ptr<ContainerPermissionsInterface> obj);

		//Call to View
		void ViewAwareObjects(
            std::function<void (const std::shared_ptr<Object>&)> func, 
            const std::shared_ptr<swganh::object::Object>& hint=nullptr);

		virtual std::shared_ptr<Object> GetContainer() = 0;
		virtual void SetContainer(const std::shared_ptr<Object>& container) = 0;

		virtual void GetAbsolutes(glm::vec3& pos, glm::quat& rot);

	protected:
		std::shared_ptr<swganh::object::ContainerPermissionsInterface> container_permissions_;

		static boost::shared_mutex global_container_lock_;

    private:
		bool __InternalHasContainedObjects();

		virtual void __InternalViewObjects(
            const std::shared_ptr<Object>& requester, 
            uint32_t max_depth, 
            bool topDown, 
            std::function<void (const std::shared_ptr<Object>&)> func) = 0;

		virtual void __InternalGetAbsolutes(glm::vec3& pos, glm::quat& rot) = 0;

		virtual void __InternalViewAwareObjects(
            std::function<void (const std::shared_ptr<Object>&)> func, 
            const std::shared_ptr<Object>& hint=nullptr) = 0;

		//FOR USE BY TRANSFER OBJECT ONLY. DO NOT CALL IN OUTSIDE CODE
		virtual void __InternalTransfer(
            const std::shared_ptr<Object>& requester,
            const std::shared_ptr<Object>& object, 
            const std::shared_ptr<Object>& newContainer,
            int32_t arrangement_id=-2) = 0;

		virtual int32_t __InternalInsert(
            const std::shared_ptr<Object>& object,
            glm::vec3 new_position,
            int32_t arrangement_id=-2) = 0;
        
		virtual void __InternalGetObjects(
            const std::shared_ptr<Object>& requester,
            uint32_t max_depth,
            bool topDown,
            std::list<std::shared_ptr<Object>>& out) = 0;
	};
}
}