// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "object.h"

#include <glm/gtx/transform2.hpp>
#include <sstream>

#include "object_events.h"

#include "swganh/logger.h"
#include "swganh/crc.h"
#include "swganh/observer/observer_interface.h"


#include "swganh_core/messages/scene_create_object_by_crc.h"
#include "swganh_core/messages/update_containment_message.h"
#include "swganh_core/messages/scene_destroy_object.h"

#include "swganh_core/messages/base_baselines_message.h"
#include "swganh_core/messages/scene_end_baselines.h"
#include "swganh_core/messages/controllers/object_menu_response.h"

#include "swganh_core/object/permissions/container_permissions_interface.h"
#include "swganh_core/simulation/player_view_box.h"

using namespace swganh::observer;
using namespace std;
using namespace swganh::object;
using namespace swganh::messages;

Object::Object()
    : object_id_(0)
	, instance_id_(0)
    , template_string_("")
    , position_(glm::vec3(0,0,0))
    , orientation_(glm::quat())
    , complexity_(0)
    , stf_name_file_("")
    , stf_name_string_("")
    , custom_name_(L"")
    , volume_(1)
	, arrangement_id_(-2)
	, database_persisted_(true)
	, in_snapshot_(false)
	, collision_height_(1.0f)
	, collision_length_(1.0f)
	, collidable_(true)
	, attributes_template_id(-1)
	, event_dispatcher_(nullptr)
	, controller_(nullptr)
	, view_box_(nullptr)
{
}

Object::~Object()
{
}

bool Object::HasController()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    return controller_ != nullptr;
}

shared_ptr<ObserverInterface> Object::GetController()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    return controller_;
}

void Object::SetController(const shared_ptr<ObserverInterface>& controller)
{
    {
	    boost::lock_guard<boost::mutex> lock(object_mutex_);
        controller_ = controller;
    }

    Subscribe(controller);
}

void Object::ClearController()
{
    shared_ptr<ObserverInterface> controller;

    {
	    boost::lock_guard<boost::mutex> lock(object_mutex_);
        controller = controller_;
        controller_.reset();
    }

    Unsubscribe(controller);
}

string Object::GetTemplate()
{
    boost::lock_guard<boost::mutex> lock(object_mutex_);
	return template_string_;
}
void Object::SetTemplate(const string& template_string)
{
    {
        boost::lock_guard<boost::mutex> lock(object_mutex_);
	    template_string_ = template_string;
    }
	DISPATCH(Object, Template);
}
void Object::SetObjectId(uint64_t object_id)
{
    object_id_ = object_id;
}
uint64_t Object::GetObjectId()
{
    return object_id_;
}

wstring Object::GetCustomName()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    return custom_name_;
}

void Object::SetCustomName(wstring custom_name)
{
    {
        boost::lock_guard<boost::mutex> lock(object_mutex_);
        custom_name_ = custom_name;
    }
    DISPATCH(Object, CustomName);
}

std::wstring Object::GetFirstName() const
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    std::size_t pos = custom_name_.find(L" ");
    return custom_name_.substr(0, pos);
}

std::wstring Object::GetSirName() const
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    std::size_t pos = custom_name_.find(L" ");
    return custom_name_.substr(pos, std::string::npos);
}

bool Object::HasObservers()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    return !observers_.empty();
}

void Object::Subscribe(const shared_ptr<ObserverInterface>& observer)
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    auto find_iter = observers_.find(observer);

    if (find_iter == observers_.end())
    {
        observers_.insert(observer);
    }
}

void Object::Unsubscribe(const shared_ptr<ObserverInterface>& observer)
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    auto find_iter = observers_.find(observer);

    if (find_iter != observers_.end())
    {
        observers_.erase(find_iter);
    }
}

void Object::NotifyObservers(swganh::messages::BaseSwgMessage* message)
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);

    std::for_each(
        observers_.begin(),
        observers_.end(),
        [&message] (const std::shared_ptr<swganh::observer::ObserverInterface>& observer)
    {
        observer->Notify(message);
    });
}

bool Object::IsDirty()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    return !deltas_.empty();
}
void Object::ClearBaselines()
{
    boost::lock_guard<boost::mutex> lock(object_mutex_);
    baselines_.clear();
}
void Object::ClearDeltas()
{
    boost::lock_guard<boost::mutex> lock(object_mutex_);
    deltas_.clear();
}

BaselinesCacheContainer Object::GetBaselines()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    return baselines_;
}

DeltasCacheContainer Object::GetDeltas(uint64_t viewer_id)
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    return deltas_;
}

void Object::AddDeltasUpdate(DeltasMessage* message)
{
    NotifyObservers(message);

	boost::lock_guard<boost::mutex> lock(object_mutex_);
    deltas_.push_back(*message);
}
void Object::AddBaselineToCache(swganh::messages::BaselinesMessage* baseline)
{
    boost::lock_guard<boost::mutex> lock(object_mutex_);
    baselines_.push_back(*baseline);
}

void Object::SetPosition(glm::vec3 position)
{
    {
	    boost::lock_guard<boost::mutex> lock(object_mutex_);
        position_ = position;
	}
	DISPATCH(Object, Position);
}

void Object::UpdatePosition(const glm::vec3& new_position, const glm::quat& orientation, shared_ptr<Object> parent)
{
	SetOrientation(orientation);
	// Call an Event that gets handled by the movement manager

	GetEventDispatcher()->Dispatch(make_shared<UpdatePositionEvent>
		("Object::UpdatePosition", parent, shared_from_this(), new_position));
}

glm::vec3 Object::GetPosition()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	return position_;
}
bool Object::InRange(glm::vec3 target, float range)
{
	if (glm::distance(GetPosition(), target) > range)
	{
		return false;
	}
	return true;
}
float Object::RangeTo(glm::vec3 target)
{
	return glm::distance(GetPosition(), target);
}
void Object::SetOrientation(glm::quat orientation)
{
    {
	    boost::lock_guard<boost::mutex> lock(object_mutex_);
        orientation_ = orientation;
    }
	DISPATCH(Object, Orientation);
}
glm::quat Object::GetOrientation()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	return orientation_;
}
void Object::FaceObject(const std::shared_ptr<Object>& object)
{
    auto target_position = object->GetPosition();
    {
	    FacePosition(target_position);
    }    
}
void Object::FacePosition(const glm::vec3& position)
{
	
    // Create a mirror direction vector for the direction we want to face.
	boost::lock_guard<boost::mutex> lock(object_mutex_);
    glm::vec3 direction_vector = glm::normalize(position - position_);
    direction_vector.x = -direction_vector.x;

    // Create a lookat matrix from the direction vector and convert it to a quaternion.
    orientation_ = glm::toQuat(glm::lookAt(
                                    direction_vector, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));                        

    // If in the 3rd quadrant the signs need to be flipped.
    if (orientation_.y <= 0.0f && orientation_.w >= 0.0f) {
        orientation_.y = -orientation_.y;
        orientation_.w = -orientation_.w; 
    }
	DISPATCH(Object, Orientation);
}

uint8_t Object::GetHeading()
{
    glm::quat tmp;
    {
	    boost::lock_guard<boost::mutex> lock(object_mutex_);
        tmp = orientation_;
    }

    float heading = 0.0f;

    if (glm::length(tmp) > 0.0f)
    {
        float s = sqrt(1 - (tmp.w * tmp.w));
        if (s != 0.0f)
        {
            if (tmp.y < 0.0f && tmp.w > 0.0f) 
            {
                tmp.y *= -1;
	        	tmp.w *= -1;
            }

			float radians = 2.0f * acos(tmp.w);
			float t = radians / 0.06283f;
			heading = (tmp.y / s) * t;
        }
    }

	return static_cast<uint8_t>(heading);
}

void Object::SetComplexity(float complexity)
{
    {
        boost::lock_guard<boost::mutex> lock(object_mutex_);
        complexity_ = complexity;
    }
	DISPATCH(Object, Complexity);
}

float Object::GetComplexity()
{
    boost::lock_guard<boost::mutex> lock(object_mutex_);
	return complexity_;
}

void Object::SetStfName(const string& stf_file_name, const string& stf_string)
{
    {
        boost::lock_guard<boost::mutex> lock(object_mutex_);
        stf_name_file_ = stf_file_name;
        stf_name_string_ = stf_string;
    }
	DISPATCH(Object, StfName);
}

string Object::GetStfNameFile()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	return stf_name_file_;
}

string Object::GetStfNameString()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	return stf_name_string_;
}

void Object::SetVolume(uint32_t volume)
{
    volume_ = volume;
	DISPATCH(Object, Volume);
}

uint32_t Object::GetVolume()
{
	return volume_;
}

void Object::SetSceneId(uint32_t scene_id)
{
    scene_id_ = scene_id;
	DISPATCH(Object, SceneId);
}

uint32_t Object::GetSceneId()
{
	return scene_id_;
}

uint32_t Object::GetInstanceId()
{
	return instance_id_;
}

void Object::SetInstanceId(uint32_t instance_id)
{
	instance_id_ = instance_id;
	DISPATCH(Object, InstanceId);
}

int32_t Object::GetArrangementId()
{
	return arrangement_id_;
}

void Object::SetArrangementId(int32_t arrangement_id)
{
	arrangement_id_ = arrangement_id;
}

swganh::EventDispatcher* Object::GetEventDispatcher()
{
    return event_dispatcher_;
}

void Object::SetEventDispatcher(swganh::EventDispatcher* dispatcher)
{
    event_dispatcher_ = dispatcher;
}

void Object::CreateBaselines( std::shared_ptr<swganh::observer::ObserverInterface> observer)
{
	if (event_dispatcher_)
	{
		GetEventDispatcher()->Dispatch(make_shared<ObserverEvent>
			("Object::Baselines", shared_from_this(), observer));
	}
}

void Object::SendCreateByCrc(std::shared_ptr<swganh::observer::ObserverInterface> observer) 
{
	//DLOG(info) << "SEND " << GetObjectId() << " TO " << observer->GetId();

	swganh::messages::SceneCreateObjectByCrc scene_object;
    scene_object.object_id = GetObjectId();
    scene_object.object_crc = swganh::memcrc(GetTemplate());
    scene_object.position = GetPosition();
	scene_object.orientation = GetOrientation();
    scene_object.byte_flag = 0;
    observer->Notify(&scene_object);

	SendUpdateContainmentMessage(observer);
}

void Object::SendUpdateContainmentMessage(std::shared_ptr<swganh::observer::ObserverInterface> observer)
{
	if(observer == nullptr)
		return;

	uint64_t container_id = 0;
	auto& container = GetContainer();
	if (container)
		container_id = GetContainer()->GetObjectId();

	//DLOG(info) << "CONTAINMENT " << GetObjectId() << ":" << GetTemplate() << " INTO " << container_id << " ARRANGEMENT " << arrangement_id_;

	UpdateContainmentMessage containment_message;
	containment_message.container_id = container_id;
	containment_message.object_id = GetObjectId();
	containment_message.containment_type = 4;
	observer->Notify(&containment_message);
}

void Object::SendDestroy(std::shared_ptr<swganh::observer::ObserverInterface> observer)
{
	//DLOG(info) << "DESTROY " << GetObjectId() << " FOR " << observer->GetId();

	swganh::messages::SceneDestroyObject scene_object;
	scene_object.object_id = GetObjectId();
	observer->Notify(&scene_object);
}

void Object::SetFlag(std::string flag)
{
    boost::lock_guard<boost::mutex> lg(object_mutex_);
    flags_.insert(flag);
}

void Object::RemoveFlag(std::string flag)
{
    boost::lock_guard<boost::mutex> lg(object_mutex_);
    flags_.erase(flag);
}

bool Object::HasFlag(std::string flag)
{
    boost::lock_guard<boost::mutex> lg(object_mutex_);

    return flags_.find(flag) != flags_.end();
}

bool Object::IsDatabasePersisted()
{
	return database_persisted_;
}

bool Object::IsInSnapshot()
{
	return in_snapshot_;
}

void Object::SetDatabasePersisted(bool value)
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	database_persisted_ = value;
}

void Object::SetInSnapshot(bool value)
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	in_snapshot_ = value;
}

AttributesMap Object::GetAttributeMap()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	return attributes_map_;
}

AttributeVariant Object::GetAttribute(const std::string& name)
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	auto find_iter = find_if(attributes_map_.begin(), attributes_map_.end(), [&](AttributesMap::value_type key_value)
	{
		return key_value.first == name;
	});
	if (find_iter != attributes_map_.end())
	{
		return find_iter->second;
	}	
	//DLOG(event) << "Attribute "<< name << " does not exist";	
	return boost::blank();
	//throw std::runtime_error("Attribute " + name + " does not exist");
}

std::wstring Object::GetAttributeAsString(const std::string& name)
{
	try {
		auto val = GetAttribute(name);		
		wstringstream attribute;
		switch (val.which())
		{
			// float
			case 0:
				attribute << boost::get<float>(val);
				break;
			case 1:
				attribute << boost::get<int64_t>(val);
				break;
			case 2:
				return boost::get<wstring>(val);
		}
	} catch (std::exception& e) {
		LOG(error) << "Attribute " << name << " could not be converted to wstring";
		throw e;
	}
	return L"";
}

int8_t Object::GetAttributeTemplateId()
{
	return attributes_template_id;
}

void Object::SetAttributeTemplateId(int8_t attribute_template_id)
{
	attributes_template_id = attribute_template_id;
}

std::wstring Object::GetAttributeRecursiveAsString(const std::string& name)
{
	auto val = GetAttributeRecursive(name);
	wstringstream ss;
	switch(val.which())
		{
			// float
			case 0:
				 ss << boost::get<float>(val);
				break;
			case 1:
				ss << boost::get<int64_t>(val);
				break;
			case 2:
				ss << boost::get<wstring>(val);
				break;
			case 3:
				ss << L"";
				break;
		}		
	
	return ss.str();
}

void Object::UpdateWorldCollisionBox(void)
{ 
	//boost::shared_lock<boost::shared_mutex> shared(global_container_lock_);
	__InternalUpdateWorldCollisionBox();
}

void Object::__InternalUpdateWorldCollisionBox()
{
	glm::vec3 pos;
	glm::quat rotation;
	GetAbsolutes(pos, rotation);
	
	auto rot = glm::yaw(rotation);
	boost::geometry::strategy::transform::translate_transformer<Point, Point> translate(pos.x, pos.z);

	if(rot <= DBL_MAX && rot >= -DBL_MAX) // glm::yaw sometimes results in a non-real float (-1.#IND) that will cause problems if not filted through.
	{
		//std::cout << "Orientation: " << rot << std::endl;
		boost::geometry::strategy::transform::rotate_transformer<Point, Point, boost::geometry::degree> rotation(rot);
		boost::geometry::strategy::transform::ublas_transformer<Point, Point, 2, 2> rotationTranslate(boost::numeric::ublas::prod(translate.matrix(), rotation.matrix()));
		boost::geometry::transform(local_collision_box_, world_collision_box_, rotationTranslate);
	}
	else
	{
		boost::geometry::transform(local_collision_box_, world_collision_box_, translate);
	}
}

AttributeVariant Object::GetAttributeRecursive(const std::string& name)
{
	auto val = GetAttribute(name);
	{
		boost::lock_guard<boost::mutex> lock(object_mutex_);
		float float_val;
		int64_t int_val;
		wstring attr_val;
		switch(val.which())
		{
			// float
			case 0:
				float_val = boost::get<float>(val);
				return AddAttributeRecursive<float>(float_val, name);			
			case 1:
				int_val = boost::get<int64_t>(val);
				return AddAttributeRecursive<int64_t>(int_val, name);			
			case 2:
				attr_val = boost::get<wstring>(val);
				return AddAttributeRecursive<wstring>(attr_val, name);			
			case 3:
				return boost::blank();				
		}	
		return boost::get<wstring>(val);	
	}
	// Doesn't Exist
	return boost::blank();
}

bool Object::HasAttribute(const std::string& name)
{
	return attributes_map_.find(name) != attributes_map_.end();
}

std::shared_ptr<Object> Object::Clone()
{
	boost::lock_guard<boost::mutex> lock(object_mutex_);
	auto other = make_shared<Object>();
	Clone(other);
	return other;
}

void Object::Clone(std::shared_ptr<Object> other)
{
	other->object_id_.store(object_id_);
	other->scene_id_.store(scene_id_);
    other->instance_id_.store(instance_id_);
	other->template_string_ = template_string_;
    other->position_ = position_;
    other->orientation_ = orientation_;
    other->complexity_ = complexity_;
    other->stf_name_file_ = stf_name_file_;
    other->stf_name_string_ = stf_name_string_;
    other->custom_name_ = custom_name_;
    other->volume_.store(volume_);
    other->arrangement_id_.store(arrangement_id_);
	other->attributes_template_id.store(attributes_template_id);
	other->attributes_map_ = attributes_map_;
	other->database_persisted_ = database_persisted_;
	other->in_snapshot_ = in_snapshot_;
    other->flags_ = flags_;

	//__InternalViewObjects(nullptr, 0, true, [&] (const std::shared_ptr<Object>& object) {
	//	other->AddObject(nullptr, object->Clone());
	//});
}

void Object::BuildSpatialProfile()
{
	BuildCollisionBox();
	BuildBoundingVolume();
}

void Object::BuildBoundingVolume()
{
	UpdateAABB();
}

void Object::BuildCollisionBox()
{
	__BuildCollisionBox();
	__InternalUpdateWorldCollisionBox();
}

void Object::UpdateAABB() 
{ 
	boost::geometry::envelope(world_collision_box_, aabb_);
}
