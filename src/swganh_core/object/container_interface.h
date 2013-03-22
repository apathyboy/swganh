// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_OBJECT_CONTAINER_INTERFACE_H_
#define SWGANH_OBJECT_CONTAINER_INTERFACE_H_

#include <cstdint>
#include <list>
#include <memory>
#include <functional>
#include <set>
#include <vector>

#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "swganh/logger.h"

#include "permissions/container_permissions_interface.h"
#include "slot_interface.h"

namespace swganh {
namespace object {
    
    typedef std::map<
	    int32_t,
	    std::shared_ptr<SlotInterface>
    > ObjectSlots;

    typedef std::vector<std::vector<int32_t>> ObjectArrangements;

    template<typename T>
	class ContainerInterface
	{
	public:
		
		virtual uint64_t GetObjectId() = 0;

		//Object Management
		void AddObject(
            const std::shared_ptr<T>& requester,
            std::shared_ptr<T> object,
            int32_t arrangement_id = -2)
        {
            if(requester == nullptr || container_permissions_->canInsert(static_cast<T*>(this)->shared_from_this(), requester, object))
        	{	
        		//boost::upgrade_lock<boost::shared_mutex> lock(global_container_lock_);
        		
        		//Add Object To Datastructure
        		{
        			//boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(lock);
        			std::shared_ptr<Object> removed_object = nullptr;
        	        if(arrangement_id == -2)
        	        	arrangement_id = static_cast<T*>(this)->GetAppropriateArrangementId(object);
        
        	        if (arrangement_id < 4)
        	        {
        	        	// Remove object in existing slot
        	        	removed_object = slot_descriptor_[arrangement_id]->insert_object(object);
        	        	if (removed_object)
        	        	{
        	        		// Transfer it out, put it in the place the replacing object came from
                            std::static_pointer_cast<ContainerInterface<T>>(object->GetContainer())->AddObject(nullptr, removed_object, -2);
        	        	}
        	        }
        	        else
        	        {
        	        	auto& arrangement = std::static_pointer_cast<ContainerInterface<T>>(object)->slot_arrangements_[arrangement_id-4];
        	        	for (auto& i : arrangement)
        	        	{
        	        		slot_descriptor_[i]->insert_object(object);
        	        	}
        	        }

        	        object->SetArrangementId(arrangement_id);
        	        object->SetContainer(static_cast<T*>(this)->shared_from_this());
        	        object->SetSceneId(static_cast<T*>(this)->GetSceneId());
        
        	        //Time to update the position to the new coordinates/update AABB
        	        //object->SetPosition(new_position);
        	        object->__InternalUpdateWorldCollisionBox();
        	        object->UpdateAABB();
        
        	        //Because we may have calculated it internally, send the arrangement_id used back
        	        //To the caller so it can send the appropriate update.
        	        //return arrangement_id;
        		}
        	}
        }

		void RemoveObject(
            const std::shared_ptr<T>& requester, 
            const std::shared_ptr<T>& oldObject)
        {
        	if(requester == nullptr || container_permissions_->canRemove(static_cast<T*>(this)->shared_from_this(), requester, oldObject))
        	{
        		//boost::upgrade_lock<boost::shared_mutex> lock(global_container_lock_);
        
        		{
        			//boost::upgrade_to_unique_lock<boost::shared_mutex> unique_lock(lock);
        			//Remove Object from Datastructure
        			for(auto& slot : slot_descriptor_)
        			{
        				slot.second->remove_object(oldObject);
        			}
        			oldObject->SetContainer(nullptr);
        		}
        	}
        }
		
        void TransferObject(
            const std::shared_ptr<T>& requester,
            const std::shared_ptr<T>& object,
            const std::shared_ptr<T>& newContainer, 
            glm::vec3 position, 
            int32_t arrangement_id=-2)
        {
        	if(	requester == nullptr || this->GetPermissions()->canRemove(static_cast<T*>(this)->shared_from_this(), requester, object))
        	{
                std::static_pointer_cast<ContainerInterface<T>>(newContainer)->AddObject(nullptr, object, arrangement_id);
        	}
        }
		
        void SwapSlots(
            const std::shared_ptr<T>& requester,
            const std::shared_ptr<T>& object,
            int32_t new_arrangement_id)
        {
        	//boost::upgrade_lock<boost::shared_mutex> uplock(global_container_lock_);
        	
        	{
        		//boost::upgrade_to_unique_lock<boost::shared_mutex> unique(uplock);
        		for(auto& slot : slot_descriptor_)
        		{
        			slot.second->remove_object(object);
        		}

        		AddObject(requester, object, new_arrangement_id);
        	}
        }

		bool HasContainedObjects()
        {
	        bool has_objects = false;
	        std::list<std::shared_ptr<T>> out;
	        GetObjects(nullptr, 1, true, out);
	        return out.size() > 0;
        }

		std::list<std::shared_ptr<T>> GetObjects(
            const std::shared_ptr<T>& requester, 
            uint32_t max_depth,
            bool topDown)
        {
	        std::list<std::shared_ptr<T>> out;
	        GetObjects(requester, max_depth, topDown, out);
	        return out;
        }

		void GetObjects(
            const std::shared_ptr<T>& requester, 
            uint32_t max_depth, 
            bool topDown, 
            std::list<std::shared_ptr<T>>& out)
        {
            ViewObjects(requester, max_depth, topDown,
                [&out, this] (const std::shared_ptr<T>& item)
            {
                out.push_back(item);
            });
        }
        
		void ViewObjects(
            const std::shared_ptr<T>& requester, 
            uint32_t max_depth,
            bool topDown, 
            std::function<void (const std::shared_ptr<T>&)> func)
        {
        	if(requester == nullptr || container_permissions_->canView(static_cast<T*>(this)->shared_from_this(), requester))
        	{
        		uint32_t requester_instance = 0;
        		if(requester)
        			requester_instance = requester->GetInstanceId();
        
        		for(auto& slot : slot_descriptor_)
        		{
        			slot.second->view_objects([&] (const std::shared_ptr<Object>& object) {
        				uint32_t object_instance = object->GetInstanceId();
        				if(object_instance == 0 || object_instance == requester_instance)
        				{
        					if(topDown)
        						func(object);
        
        					if(max_depth != 1)
        						std::static_pointer_cast<ContainerInterface<T>>(object)->ViewObjects(requester, (max_depth == 0) ? 0 : max_depth-1, topDown, func);
        
        					if(!topDown)
        						func(object);
        				}
        			});
        		}
        	}
        }

		std::shared_ptr<ContainerPermissionsInterface> GetPermissions()
        { 
	        return container_permissions_; 
        }

		void SetPermissions(std::shared_ptr<ContainerPermissionsInterface> obj)
        { 
	        container_permissions_ = obj; 
        }
        
		virtual std::shared_ptr<Object> GetContainer() = 0;
		virtual void SetContainer(const std::shared_ptr<T>& container) = 0;

		void GetAbsolutes(glm::vec3& pos, glm::quat& rot)
        {
	        //boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);	        
	        auto parentContainer = GetContainer();
	        if(parentContainer)
	        {
	        	 std::static_pointer_cast<ContainerInterface<T>>(parentContainer)->GetAbsolutes(pos, rot);
	        }
	        else
	        {
	        	pos = glm::vec3();
	        	rot = glm::quat();
	        }

	        pos = (rot * static_cast<T*>(this)->GetPosition()) + pos;
	        rot = rot * static_cast<T*>(this)->GetOrientation();
        }
        
        /// Slots
        
        void SetSlotInformation(ObjectSlots slots, ObjectArrangements arrangements)
        {
        	slot_descriptor_ = slots;
        	slot_arrangements_ = arrangements;
        }
        
        int32_t GetAppropriateArrangementId(std::shared_ptr<Object> other)
        {
        	if (slot_descriptor_.size() == 1)
        		return -1;
        
        	// Find appropriate arrangement
        	int32_t arrangement_id = 4;
        	int32_t filled_arrangement_id = -1;
        	// In each arrangment
        	for ( auto& arrangement : std::static_pointer_cast<ContainerInterface<T>>(other)->slot_arrangements_)
        	{
        		bool passes_completely = true;
        		bool is_valid = true;
        		// Each Slot
        		for (auto& slot : arrangement)
        		{
        			// does slot exist in descriptor
        			auto descriptor_iter = slot_descriptor_.find(slot);
        			if (descriptor_iter == end(slot_descriptor_))
        			{
        				is_valid = false;
        				break;
        			}
        			// is slot filled?
        			else if (descriptor_iter->second->is_filled())
        			{
        				passes_completely = false;
        			}
        		}
        		// if not filled return arrangement
        		if (is_valid && passes_completely)
        			return arrangement_id;
        		else if (is_valid)
        			filled_arrangement_id = arrangement_id;
        
        		++arrangement_id;
        	}
        	return filled_arrangement_id;
        }
        
        ObjectSlots GetSlotDescriptor()
        {
        	return slot_descriptor_;
        }

        ObjectArrangements GetSlotArrangements()
        {
        	return slot_arrangements_;
        }

        bool ClearSlot(int32_t slot_id)
        {
        	bool cleared = false;
        	auto slot_iter = slot_descriptor_.find(slot_id);
        	if (slot_iter != slot_descriptor_.end())
        	{
        		auto slot = slot_iter->second;
        		if (!slot->is_filled())
        		{
        			slot->view_objects([&](std::shared_ptr<Object> object){
        				slot->remove_object(object);
        				cleared = true;
        			});
        			
        		}
        	}
        	return cleared;
        }

        std::shared_ptr<Object> GetSlotObject(int32_t slot_id)
        {
        	std::shared_ptr<Object> found = nullptr;
        	auto slot_iter = slot_descriptor_.find(slot_id);
        	if (slot_iter != slot_descriptor_.end())
        	{
        		auto slot = slot_iter->second;
        		slot->view_objects([&](std::shared_ptr<Object> object){
        			found = object;
        		});
        	}
        	return found;
        }
        
    private:

        ObjectSlots slot_descriptor_;
	    ObjectArrangements slot_arrangements_;
		std::shared_ptr<swganh::object::ContainerPermissionsInterface> container_permissions_;
	};

}}

#endif
