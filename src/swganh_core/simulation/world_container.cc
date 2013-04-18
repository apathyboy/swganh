// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "world_container.h"

#include "swganh_core/simulation/spatial_provider_interface.h"
#include "swganh_core/simulation/player_view_box.h"

using namespace swganh::simulation;

void WorldContainer::AddObject(std::shared_ptr<swganh::object::Object> requester, std::shared_ptr<swganh::object::Object> newObject, int32_t arrangement_id) 
{ 
	std::cout << "WorldContainer::AddObject" << std::endl; 
	spatial_provider_->AddObject(newObject, arrangement_id);
}
	
void WorldContainer::RemoveObject(std::shared_ptr<swganh::object::Object> requester, std::shared_ptr<swganh::object::Object> oldObject) 
{ 
	std::cout << "WorldContainer::RemoveObject" << std::endl; 
	spatial_provider_->RemoveObject(oldObject);
}
	
void WorldContainer::TransferObject(std::shared_ptr<swganh::object::Object> requester, std::shared_ptr<swganh::object::Object> object, std::shared_ptr<ContainerInterface> newContainer, glm::vec3 position, int32_t arrangement_id) 
{ 
	std::cout << "WorldContainer::TransferObject" << std::endl; 
	boost::upgrade_lock<boost::shared_mutex> uplock(global_container_lock_);
	{
#ifdef SI_METHOD_TWO
		spatial_provider_->DeleteObject(object);
#endif
	arrangement_id = newContainer->__InternalInsert(object, position, arrangement_id);
	}

	//Send updates to both
	auto controllersInView = object->GetControllersInView();
	for(auto& observer : controllersInView) {
		object->SendUpdateContainmentMessage(observer);
	}
}

int32_t WorldContainer::__InternalInsert(std::shared_ptr<swganh::object::Object> object, glm::vec3 new_position, int32_t arrangement)
{
	std::cout << "WorldContainer::__InternalInsert" << std::endl;
	object->SetContainer(spatial_provider_->GetWorldContainer());
	object->SetPosition(new_position);
	object->__InternalUpdateWorldCollisionBox();
	object->UpdateAABB();
#ifdef SI_METHOD_TWO
	spatial_provider_->InsertObject(object);
#endif
	return -1;
}