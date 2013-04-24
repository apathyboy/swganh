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
    player_->AddAware(collider);
}

void PlayerViewBox::OnCollisionStay(std::shared_ptr<Object> collider)
{}

void PlayerViewBox::OnCollisionLeave(std::shared_ptr<Object> collider)
{
    player_->RemoveAware(collider);
}