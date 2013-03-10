// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <swganh_core/simulation/player_view_box.h>

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
	}
}