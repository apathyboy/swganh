// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "movement_manager.h"

#include "swganh/logger.h"

#include "swganh/event_dispatcher.h"

#include "swganh/app/swganh_kernel.h"
#include "swganh/service/service_manager.h"
#include "swganh_core/object/object.h"
#include "swganh_core/object/object_events.h"
#include "swganh_core/object/creature/creature.h"
#include "swganh/observer/observer_interface.h"

#include "swganh_core/messages/update_containment_message.h"
#include "swganh_core/messages/update_transform_message.h"
#include "swganh_core/messages/update_transform_with_parent_message.h"

#include "swganh_core/simulation/spatial_provider_interface.h"
#include "swganh_core/simulation/simulation_service_interface.h"
#include "swganh_core/simulation/player_view_box.h"

using namespace swganh::event_dispatcher;
using namespace std;
using namespace swganh::messages;
using namespace swganh::messages::controllers;
using namespace swganh::object;
using namespace swganh::object;
using namespace swganh::simulation;
using namespace swganh::simulation;

MovementManager::MovementManager(swganh::app::SwganhKernel* kernel, std::string scene_name)
	: kernel_(kernel)
	, scene_name_(scene_name)
{
	simulation_service_ = kernel_->GetServiceManager()->GetService<SimulationServiceInterface>("SimulationService");

	RegisterEvents(kernel_->GetEventDispatcher());
}

void MovementManager::HandleDataTransformServer(
    const shared_ptr<Object>& object,
	const glm::vec3& new_position)
{
    counter_map_[object->GetObjectId()] = counter_map_[object->GetObjectId()];
	auto& view_box = object->GetViewBox();

	AABB old_bounding_volume = object->GetAABB();
	AABB view_box_old_bounding_volume = view_box->GetAABB();

	//If the object was inside a container we need to move it out
	/* if(object->GetContainer() != spatial_provider_)
	{
		std::shared_ptr<Object> old_container = std::static_pointer_cast<Object>(object->GetContainer());
		if(old_container->GetTemplate().compare("object/cell/shared_cell.iff") == 0) 
		{
			LOG(warning) << "Entering Cells... cells are under construction.";
		} 
		else 
		{
			AABB old_parent_bounding_volume = old_container->GetAABB();
			old_container->SetPosition(new_position);
			old_container->UpdateWorldCollisionBox();
			old_container->UpdateAABB();

			object->UpdateWorldCollisionBox();
			object->UpdateAABB();
			
			spatial_provider_->UpdateObject(old_container, old_parent_bounding_volume, old_container->GetAABB());
			SendDataTransformMessage(old_container);
		}
	}
	else
	{*/
		object->SetPosition(new_position);
		view_box->SetPosition(object->GetPosition());

		object->UpdateWorldCollisionBox();
		object->UpdateAABB();
		view_box->UpdateWorldCollisionBox();
		view_box->UpdateAABB();

		spatial_provider_->UpdateObject(object, old_bounding_volume, object->GetAABB(), view_box, view_box_old_bounding_volume, view_box->GetAABB());
		SendDataTransformMessage(object);
	//}
}

void MovementManager::HandleDataTransformWithParentServer(
    const shared_ptr<Object>& parent, 
    const shared_ptr<Object>& object,
	const glm::vec3& new_position)
	
{
	if(parent != nullptr)
	{
        object->SetPosition(new_position);
	    object->UpdateWorldCollisionBox();
		object->UpdateAABB();

		//Perform the transfer if needed
		if(object->GetContainer() != parent)
		{
			object->GetContainer()->TransferObject(object, object, parent);
		}
				
		//Send the update transform
		SendDataTransformWithParentMessage(object);

	}
	else
	{
		HandleDataTransformServer(object, new_position);
	}
}

void MovementManager::HandleDataTransform(
    const shared_ptr<Object>& object, 
    DataTransform message)
{    
    if (!ValidateCounter_(object->GetObjectId(), message.counter))
    {
		LOG(error) << "Movement Counter is " << message.counter << " should be " << (counter_map_[object->GetObjectId()] + 1) << ".";
        return;
    }

    counter_map_[object->GetObjectId()] = message.counter;

	auto& view_box = object->GetViewBox();
	AABB view_box_old_bounding_volume = view_box->GetAABB();
	AABB old_bounding_volume = object->GetAABB();

	//If the object was inside a container we need to move it out
	/* if(object->GetContainer() != spatial_provider_)
	{
		std::shared_ptr<Object> old_container = std::static_pointer_cast<Object>(object->GetContainer());
		if(old_container->GetTemplate().compare("object/cell/shared_cell.iff") == 0) 
		{
			LOG(warning) << "Entering cell.... cells are underconstruction.";
		} 
		else 
		{
			AABB old_parent_bounding_volume = old_container->GetAABB();
			old_container->SetOrientation(message.orientation);
			old_container->SetPosition(message.position);
			old_container->UpdateWorldCollisionBox();
			old_container->UpdateAABB();
			
			object->UpdateWorldCollisionBox();
			object->UpdateAABB();

			spatial_provider_->UpdateObject(old_container, old_parent_bounding_volume, old_container->GetAABB());
			SendUpdateDataTransformMessage(old_container);
		}
	}
	else
	{*/
		object->SetPosition(message.position);
		object->SetOrientation(message.orientation);
		view_box->SetPosition(object->GetPosition());
		object->UpdateWorldCollisionBox();
		object->UpdateAABB();
		view_box->UpdateWorldCollisionBox();
		view_box->UpdateAABB();
		spatial_provider_->UpdateObject(object, old_bounding_volume, object->GetAABB(), view_box, view_box_old_bounding_volume, view_box->GetAABB());
		SendUpdateDataTransformMessage(object);
	//}
}

void MovementManager::HandleDataTransformWithParent(
    const shared_ptr<Object>& object, 
    DataTransformWithParent message)
{
    return; // to implement....
	auto container = simulation_service_->GetObjectById(message.cell_id);
	if(container != nullptr)
	{
		if (!ValidateCounter_(object->GetObjectId(), message.counter))
		{
			LOG(error) << "Movement Counter is " << message.counter << " should be " << (counter_map_[object->GetObjectId()] + 1) << ".";
			return;
		}

		counter_map_[object->GetObjectId()] = message.counter;

		//Set the new position and orientation
		object->SetOrientation(message.orientation);
        object->SetPosition(message.position);
		object->UpdateWorldCollisionBox();
		object->UpdateAABB();

		//Perform the transfer
		if(object->GetContainer() != container)
		{
			object->GetContainer()->TransferObject(object, object, container);
		}

		//Send the update transform
		SendUpdateDataTransformWithParentMessage(object);
	}
	else
	{
		LOG(error) << "Cell ID: " << message.cell_id << " not found.";
	}
}

void MovementManager::SendDataTransformMessage(const shared_ptr<Object>& object, uint32_t unknown)
{
    auto creature = static_pointer_cast<Creature>(object);

    DataTransform transform;
    transform.counter = ++counter_map_[object->GetObjectId()];
    transform.orientation = object->GetOrientation();
    transform.position = object->GetPosition();
    transform.speed = creature->GetWalkingSpeed();

    object->NotifyObservers(&transform);
}

void MovementManager::SendUpdateDataTransformMessage(const shared_ptr<Object>& object)
{
    UpdateTransformMessage transform_update;
    transform_update.object_id = object->GetObjectId();
    transform_update.heading = object->GetHeading();
    transform_update.position = object->GetPosition();
    transform_update.update_counter = ++counter_map_[object->GetObjectId()];
    
    object->NotifyObservers(&transform_update);
}

void MovementManager::SendDataTransformWithParentMessage(const shared_ptr<Object>& object, uint32_t unknown)
{    
    auto creature = static_pointer_cast<Creature>(object);

    DataTransformWithParent transform;
    transform.cell_id       = object->GetContainer()->GetContainmentId();
    transform.counter       = ++counter_map_[object->GetObjectId()];
    transform.orientation   = object->GetOrientation();
    transform.position      = object->GetPosition();
    transform.speed         = creature->GetWalkingSpeed();

    object->NotifyObservers(&transform);
}

void MovementManager::SendUpdateDataTransformWithParentMessage(const shared_ptr<Object>& object)
{    
    UpdateTransformWithParentMessage transform_update;
    transform_update.object_id = object->GetObjectId();
    transform_update.cell_id = object->GetContainer()->GetContainmentId();
    transform_update.heading = object->GetHeading();
    transform_update.position = object->GetPosition();
    transform_update.update_counter = ++counter_map_[object->GetObjectId()];
    
    object->NotifyObservers(&transform_update);
}

void MovementManager::RegisterEvents(swganh::EventDispatcher* event_dispatcher)
{
    event_dispatcher->Subscribe(
        "ObjectReadyEvent",
        [this] (shared_ptr<swganh::EventInterface> incoming_event)
    {
        const auto& object = static_pointer_cast<swganh::ValueEvent<shared_ptr<Object>>>(incoming_event)->Get();

		counter_map_[object->GetObjectId()] = 0;
    });

	event_dispatcher->Subscribe(
		"SpatialIndexSvgDump",
		[this] (shared_ptr<swganh::EventInterface> incoming_event)
	{
		spatial_provider_->SvgToFile();
	});
}

bool MovementManager::ValidateCounter_(uint64_t object_id, uint32_t counter)
{    
    return counter >= counter_map_[object_id];
}

void MovementManager::SetSpatialProvider(std::shared_ptr<swganh::simulation::SpatialProviderInterface> spatial_provider)
{
	spatial_provider_ = spatial_provider;
}
