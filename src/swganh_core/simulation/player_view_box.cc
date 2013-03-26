// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <swganh_core/simulation/player_view_box.h>
#include <swganh_core/object/permissions/default_permission.h>

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


void PlayerViewBox::OnCollisionEnter(std::shared_ptr<Object> collider)
{
	// We only process top level objects, contained objects will
	// be spawned along with top level objects to ensure correct
	// ordering.
	if(collider->GetContainer() != nullptr)
		if(collider->GetContainer()->GetObjectId() != 0)
			return;

	// Add collided object to view.
	auto& controller = player_->GetController();
	if(controller != nullptr)
	{
		if(collider->IsInSnapshot() == false)
		{
			std::cout << "Creating [ " << collider->GetTemplate() << ":" << collider->GetObjectId(); 
			if(collider->GetContainer() != nullptr)
				std::cout << ":" << collider->GetContainer()->GetObjectId();
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
						std::cout << ":" << child->GetContainer()->GetObjectId();
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
	// We only process top level objects, contained objects will
	// be spawned along with top level objects to ensure correct
	// ordering.
	if(collider->GetContainer() != nullptr)
		if(collider->GetContainer()->GetObjectId() != 0)
			return;

	// Remove collided object from view.
	auto& controller = player_->GetController();
	if(controller != nullptr)
	{
		if(collider->IsInSnapshot() == false)
		{
			std::cout << "Destroying [ " << collider->GetTemplate() << ":" << collider->GetObjectId(); 
			if(collider->GetContainer() != nullptr)
				std::cout << ":" << collider->GetContainer()->GetObjectId();
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
				std::cout << ":" << child->GetContainer()->GetObjectId();
			std::cout << "] for ";
			std::wcout << player_->GetCustomName() << std::endl;
			child->SendDestroy(controller);
			child->Unsubscribe(controller);
		}
		});
	}
}