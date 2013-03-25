// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <swganh_core/simulation/player_view_box.h>

#include "swganh/logger.h"

using namespace swganh::simulation;
using namespace swganh::object;

static double VIEWING_RANGE = 128.0f;

PlayerViewBox::PlayerViewBox(std::shared_ptr<Object> player)
	: Object()
	, player_(player)
{
	SetCollisionBoxSize((float)VIEWING_RANGE * 2.0f,(float) VIEWING_RANGE * 2.0f);
}

PlayerViewBox::~PlayerViewBox()
{
}


void PlayerViewBox::OnCollisionEnter(std::shared_ptr<Object> collider)
{
	// Add collided object to view.
	auto& controller = player_->GetController();
	if(controller != nullptr && collider->IsInSnapshot() == false)
	{
		std::cout << "Creating [ " << collider->GetTemplate() << "] for ";
		std::wcout << player_->GetCustomName() << std::endl;
		collider->Subscribe(controller);
		collider->SendCreateByCrc(controller);
		collider->CreateBaselines(controller);

        bool self_collision = (player_ == collider) ? true : false;

        auto slots = collider->GetSlotDescriptor();
        for(auto& slot : slots)
        {
            slot.second->view_objects([&controller, self_collision, this] (const std::shared_ptr<Object>& object)
            {
                object->Subscribe(controller);
                object->SendCreateByCrc(controller);
                object->CreateBaselines(controller);

                if(self_collision)
                {
                    object->ViewObjects(player_, 0, true, [&controller] (const std::shared_ptr<Object>& object)
                    {
                        object->Subscribe(controller);
                        object->SendCreateByCrc(controller);
                        object->CreateBaselines(controller);
                    });
                }
            });
        }
	}
}

void PlayerViewBox::OnCollisionLeave(std::shared_ptr<Object> collider)
{
	// Remove collided object from view.
	auto& controller = player_->GetController();
	if(controller != nullptr && collider->IsInSnapshot() == false)
	{
		std::cout << "Destroying [ " << collider->GetTemplate() << "] for ";
		std::wcout << player_->GetCustomName() << std::endl;
		collider->SendDestroy(controller);
		collider->Unsubscribe(controller);

        bool self_collision = (player_ == collider) ? true : false;
                
        auto slots = collider->GetSlotDescriptor();
        for(auto& slot : slots)
        {
            slot.second->view_objects([&controller, self_collision, this] (const std::shared_ptr<Object>& object)
            {
                object->SendDestroy(controller);
                object->Unsubscribe(controller);

                if(self_collision)
                {
                    object->ViewObjects(player_, 0, true, [&controller] (const std::shared_ptr<Object>& object)
                    {
                        object->SendDestroy(controller);
                        object->Unsubscribe(controller);
                    });
                }
            });
        }
	}
}