// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <swganh_core/simulation/player_view_box.h>
#include <swganh_core/object/permissions/default_permission.h>

#include "swganh/logger.h"

using namespace swganh::simulation;
using namespace swganh::object;

static double VIEWING_RANGE = 128.0f;

PlayerViewBox::PlayerViewBox(std::shared_ptr<Object> player)
	: Object()
	, player_(player)
{
	SetInSnapshot(true); // We don't actually spawn this object.
	SetDatabasePersisted(false); // We don't store it.
	SetCollidable(true);
	SetCollisionBoxSize((float)VIEWING_RANGE * 2.0f,(float) VIEWING_RANGE * 2.0f);
	SetTemplate("server/anh_view_box");
	auto abs_position = glm::vec3();
	auto abs_orientation = glm::quat();
	player->GetAbsolutes(abs_position, abs_orientation);
	SetPosition(abs_position);
	SetObjectId(player->GetObjectId() + 16);
	SetPermissions(std::make_shared<swganh::object::DefaultPermission>());
}

PlayerViewBox::~PlayerViewBox()
{
}

/**
 * Whenever a new object collides with our view box, the following procedure happens:
 * 
 * - Collided object and player are made "aware" of each other.
 * - Check to see if we are in a container, if we are:
 *   - If our parent is already spawned or is a snapshot object, spawn the object.
 * - Check to see if our player has a controller, if so spawn the object (and relevant children) if it's not in the world snapshot.
 */
void PlayerViewBox::OnCollisionEnter(std::shared_ptr<Object> collider)
{
	// At the very least, we are aware of this object now.
	aware_of_.insert(collider);

	// We only process top level objects, contained objects will
	// be spawned along with top level objects to ensure correct
	// ordering.
	if(collider->GetContainer() != nullptr)
	{
        if(collider->GetContainer()->GetContainmentId() != 0)
		{
			// We are already aware of the parent, this is a new object spawned
			// after the parent.
			if(aware_of_.find(std::static_pointer_cast<swganh::object::Object>(collider->GetContainer())) != aware_of_.end() 
				&&
				aware_of_.find(std::static_pointer_cast<swganh::object::Object>(collider)) == aware_of_.end())
			{
				// Add collided object to view.
				auto& controller = player_->GetController();
				if(controller != nullptr)
				{
					if(collider->IsInSnapshot() == false)
					{
						std::cout << "Creating2 [ " << collider->GetTemplate() << ":" << collider->GetObjectId(); 
						if(collider->GetContainer() != nullptr)
                            std::cout << ":" << collider->GetContainer()->GetContainmentId();
						std::cout << "] for ";
						std::wcout << player_->GetCustomName() << std::endl;
						collider->Subscribe(controller);
						collider->SendCreateByCrc(controller);
						collider->CreateBaselines(controller);
					}

					collider->ViewObjects(nullptr, 0, true, [=](const std::shared_ptr<Object>& child) {
							if(child->IsInSnapshot() == false)
							{
								std::cout << "Creating2 [ " << child->GetTemplate() << ":" << child->GetObjectId(); 
								if(child->GetContainer() != nullptr)
									std::cout << ":" << child->GetContainer()->GetContainmentId();
								std::cout << "] for ";
								std::wcout << player_->GetCustomName() << std::endl;
								child->Subscribe(controller);
								child->SendCreateByCrc(controller);
								child->CreateBaselines(controller);
							}
					});
				}
			}

			return;
		}
	}

	// Add collided object to view.
	auto& controller = player_->GetController();
	if(controller != nullptr)
	{
		if(collider->IsInSnapshot() == false)
		{
			std::cout << "Creating [ " << collider->GetTemplate() << ":" << collider->GetObjectId(); 
			if(collider->GetContainer() != nullptr)
				std::cout << ":" << collider->GetContainer()->GetContainmentId();
			std::cout << "] for ";
			std::wcout << player_->GetCustomName() << std::endl;
			collider->Subscribe(controller);
			collider->SendCreateByCrc(controller);
			collider->CreateBaselines(controller);
		}

		collider->ViewObjects(nullptr, 0, true, [=](const std::shared_ptr<Object>& child) {
				if(child->IsInSnapshot() == false)
				{
					std::cout << "Creating [ " << child->GetTemplate() << ":" << child->GetObjectId(); 
					if(child->GetContainer() != nullptr)
						std::cout << ":" << child->GetContainer()->GetContainmentId();
					std::cout << "] for ";
					std::wcout << player_->GetCustomName() << std::endl;
					child->Subscribe(controller);
					child->SendCreateByCrc(controller);
					child->CreateBaselines(controller);
				}
		});
	}
}

void PlayerViewBox::OnCollisionStay(std::shared_ptr<Object> collider)
{
}

void PlayerViewBox::OnCollisionLeave(std::shared_ptr<Object> collider)
{
	aware_of_.erase(collider);

	// We only process top level objects, contained objects will
	// be spawned along with top level objects to ensure correct
	// ordering.
	if(collider->GetContainer() != nullptr)
		if(collider->GetContainer()->GetContainmentId() != 0)
			return;

	// Remove collided object from view.
	auto& controller = player_->GetController();
	if(controller != nullptr)
	{
		if(collider->IsInSnapshot() == false)
		{
			std::cout << "Destroying [ " << collider->GetTemplate() << ":" << collider->GetObjectId(); 
			if(collider->GetContainer() != nullptr)
				std::cout << ":" << collider->GetContainer()->GetContainmentId();
			std::cout << "] for ";
			std::wcout << player_->GetCustomName() << std::endl;
			collider->SendDestroy(controller);
			collider->Unsubscribe(controller);
		}

		collider->ViewObjects(nullptr, 0, true, [=](const std::shared_ptr<Object>& child) {
		if(child->IsInSnapshot() == false)
		{	
			std::cout << "Destroying [ " << child->GetTemplate() << ":" << child->GetObjectId(); 
			if(child->GetContainer() != nullptr)
				std::cout << ":" << child->GetContainer()->GetContainmentId();
			std::cout << "] for ";
			std::wcout << player_->GetCustomName() << std::endl;
			child->SendDestroy(controller);
			child->Unsubscribe(controller);
		}
		});
	}
}