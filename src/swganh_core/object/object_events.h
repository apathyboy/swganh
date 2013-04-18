// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "swganh/event_dispatcher.h"
#include "object.h"

namespace swganh {
namespace object {

struct ControllerEvent : swganh::BaseEvent
{
    ControllerEvent(swganh::EventType type, std::shared_ptr<swganh::object::Object> object_, std::shared_ptr<swganh::object::ObjectController> controller_)
        : BaseEvent(type)
        , object(object_)
        , controller(controller_)
    {}
    std::shared_ptr<swganh::object::Object> object;
    std::shared_ptr<swganh::object::ObjectController> controller;
};

struct ObserverEvent : swganh::BaseEvent
{
	ObserverEvent(swganh::EventType type, std::shared_ptr<swganh::object::Object> object_, std::shared_ptr<swganh::observer::ObserverInterface> observer_)
        : BaseEvent(type)
        , object(object_)
        , observer(observer_)
    {}
    std::shared_ptr<swganh::object::Object> object;
    std::shared_ptr<swganh::observer::ObserverInterface> observer;
};

struct UpdatePositionEvent : swganh::BaseEvent
{
	UpdatePositionEvent(swganh::EventType type, std::shared_ptr<swganh::object::Object> parent_, std::shared_ptr<swganh::object::Object> object_, glm::vec3 new_position)
		: BaseEvent(type)
        , object(object_)
		, parent(parent_)
		, position(new_position)
	{}

	std::shared_ptr<swganh::object::Object> object;
	std::shared_ptr<swganh::object::Object> parent;
	glm::vec3 position;
};

struct SetPositionEvent : swganh::BaseEvent
{
	SetPositionEvent(swganh::EventType type, std::shared_ptr<swganh::object::Object> object_, glm::vec3 old_position_, swganh::object::AABB old_bounding_volume_, glm::vec3 new_position_, swganh::object::AABB new_bounding_volume_)
		: BaseEvent(type)
		, object(object_)
		, old_bounding_volume(old_bounding_volume_)
		, new_bounding_volume(new_bounding_volume_)
		, old_position(old_position_)
		, new_position(new_position_)
	{}

	std::shared_ptr<swganh::object::Object> object;
	swganh::object::AABB old_bounding_volume;
	swganh::object::AABB new_bounding_volume;
	glm::vec3 old_position;
	glm::vec3 new_position;
};

}} // swganh::object