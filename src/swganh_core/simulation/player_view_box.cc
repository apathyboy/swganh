// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <swganh_core/simulation/player_view_box.h>

using namespace swganh::simulation;
using namespace swganh::object;

static double VIEWING_RANGE = 150.0f;

PlayerViewBox::PlayerViewBox(std::shared_ptr<Object> player)
	: Object()
	, player_(player)
{
	SetCollisionBoxSize((float)VIEWING_RANGE,(float) VIEWING_RANGE);
}

PlayerViewBox::~PlayerViewBox()
{
}


void PlayerViewBox::OnCollisionEnter(std::shared_ptr<Object> collider)
{
	auto& controller = player_->GetController();
	if(controller != nullptr)
	{
		collider->Subscribe(controller);
		collider->SendCreateByCrc(controller);
		collider->CreateBaselines(controller);
	}


	player_->AddAwareObject(collider);
}

void PlayerViewBox::OnCollisionLeave(std::shared_ptr<Object> collider)
{
	auto& controller = player_->GetController();
	if(controller != nullptr)
	{
		collider->Unsubscribe(controller);
		collider->SendDestroy(controller);
	}

	player_->RemoveAwareObject(collider);
}