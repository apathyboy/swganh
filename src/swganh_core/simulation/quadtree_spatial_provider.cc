// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "quadtree_spatial_provider.h"

#include "swganh/logger.h"

#include "swganh_core/object/object.h"
#include "swganh_core/object/permissions/world_permission.h"

using std::shared_ptr;

using namespace swganh::observer;
using namespace swganh::object;
using namespace swganh::simulation;
using namespace quadtree;

static int VIEWING_RANGE = 128;

QuadtreeSpatialProvider::QuadtreeSpatialProvider()
	: root_node_(ROOT, Region(quadtree::Point(-8300.0f, -8300.0f), 
	quadtree::Point(8300.0f, 8300.0f)), 0, 9, nullptr)
{
}

QuadtreeSpatialProvider::~QuadtreeSpatialProvider(void)
{
}

void QuadtreeSpatialProvider::AddObject(shared_ptr<Object> object, int32_t arrangement_id)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);
	root_node_.InsertObject(object);
	object->SetArrangementId(arrangement_id);
	object->SetSceneId(scene_id_);

	CheckCollisions(object);

	// Add our children.
	object->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		root_node_.InsertObject(child);
		child->SetSceneId(scene_id_);

		CheckCollisions(child);
	});
}

void QuadtreeSpatialProvider::RemoveObject(shared_ptr<Object> object)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);

	// Remove our children.
	object->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		auto& collided_objects = child->GetCollidedObjects();
		root_node_.RemoveObject(child);

		for(auto& collided_object : collided_objects)
		{
			collided_object->OnCollisionLeave(object);
		}
	});

	auto& collided_objects = object->GetCollidedObjects();
	root_node_.RemoveObject(object);

	// 
	for(auto& collided_object : collided_objects)
	{
		collided_object->OnCollisionLeave(object);
	}
}

void QuadtreeSpatialProvider::UpdateObject(shared_ptr<Object> obj, const swganh::object::AABB& old_bounding_volume, const swganh::object::AABB& new_bounding_volume, std::shared_ptr<swganh::object::Object> view_box,
										   const swganh::object::AABB view_box_old_bounding_volume, const swganh::object::AABB view_box_new_bounding_volume)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);

	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	root_node_.UpdateObject(obj, old_bounding_volume, new_bounding_volume);
	
	// Update children
	obj->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		AABB old_aabb = child->GetAABB();
		child->UpdateWorldCollisionBox();
		child->UpdateAABB();
		root_node_.UpdateObject(obj, old_bounding_volume, child->GetAABB());
	});

	if(view_box != nullptr) {
		root_node_.UpdateObject(view_box, view_box_old_bounding_volume, view_box_new_bounding_volume);
		CheckCollisions(view_box);
	}

	obj->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		CheckCollisions(child);
	});

	CheckCollisions(obj);

	auto stop_time = std::chrono::high_resolution_clock::now();

   // std::cout << "SpatialProvider::UpdateObject Duration: " << 
       // std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() 
        //<< "ms" << std::endl;

}

std::set<std::shared_ptr<swganh::object::Object>> QuadtreeSpatialProvider::Query(boost::geometry::model::polygon<swganh::object::Point> query_box)
{
	std::set<std::shared_ptr<swganh::object::Object>> return_vector;
	QueryBox aabb;

	boost::geometry::envelope(query_box, aabb);

	return_vector = root_node_.Query(aabb); // Find objects without our AABB
	for(auto i = return_vector.begin(); i != return_vector.end(); i++)
	{
		// Do more precise intersection detection, if we are not colliding, erase.
		if(boost::geometry::intersects((*i)->GetWorldCollisionBox(), query_box) == false)
			i = return_vector.erase(i);
	}

	return return_vector;
}

std::set<std::shared_ptr<swganh::object::Object>> QuadtreeSpatialProvider::Query(glm::vec3 position, float radius)
{
	std::set<std::shared_ptr<swganh::object::Object>> return_vector;
	
	AABB aabb;
	QueryBox query_box;

	if(radius <= 0)
		query_box = QueryBox(quadtree::Point(1.0f - VIEWING_RANGE, 1.0f - VIEWING_RANGE), 
					quadtree::Point(1.0f + VIEWING_RANGE, 1.0f + VIEWING_RANGE));
	else
		query_box = QueryBox(quadtree::Point(1.0f - VIEWING_RANGE, 1.0f - VIEWING_RANGE), 
					quadtree::Point(1.0f + VIEWING_RANGE, 1.0f + VIEWING_RANGE));

	boost::geometry::envelope(query_box, aabb);

	return_vector = root_node_.Query(aabb); // Find objects with our AABB
	for(auto i = return_vector.begin(); i != return_vector.end(); i++)
	{
		// Do more precise intersection detection, if we are not colliding, erase.
		if(boost::geometry::intersects((*i)->GetWorldCollisionBox(), query_box) == false)
			i = return_vector.erase(i);
	}

	return return_vector;
}

std::set<std::pair<float, std::shared_ptr<swganh::object::Object>>> QuadtreeSpatialProvider::FindObjectsInRangeByTag(std::shared_ptr<swganh::object::Object> requester, const std::string& tag, float range)
{
	std::set<std::pair<float, std::shared_ptr<swganh::object::Object>>> obj_map;

	QueryBox query_box;
	if(range < 0) // Query the whole map.
		query_box = QueryBox(quadtree::Point(-8300.0f, -8300.0f), quadtree::Point(8300.0f, 8300.0f));
	else // Generate query box.
		query_box = QueryBox(
			quadtree::Point(requester->GetPosition().x - (range /  2.0f), requester->GetPosition().z - (range / 2.0f)),
			quadtree::Point(requester->GetPosition().x + (range / 2.0f), requester->GetPosition().z + (range / 2.0f))
			);

	auto objects = root_node_.Query(query_box);
	for(auto& object : objects)
	{
		if(object->HasFlag(tag))
		{
			glm::vec3 pos1, pos2;
			requester->GetAbsolutes(pos1, glm::quat());
			object->GetAbsolutes(pos2, glm::quat());

			obj_map.insert(std::pair<float, std::shared_ptr<swganh::object::Object>>(glm::distance(pos1, pos2),object));
			object->ViewObjects(object, 0, true, [=, &obj_map](std::shared_ptr<Object> object) {
				glm::vec3 pos1, pos2;
				if(object->HasFlag(tag))
				{
					requester->GetAbsolutes(pos1, glm::quat());
					object->GetAbsolutes(pos2, glm::quat());
					obj_map.insert(std::pair<float, std::shared_ptr<swganh::object::Object>>(glm::distance(pos1, pos2), object));
				}
			});
		}
	}
	return obj_map;
}

void QuadtreeSpatialProvider::CheckCollisions(std::shared_ptr<swganh::object::Object> object)
{
	//if(object->IsInSnapshot() == true)// || object->IsCollidable() == false)
		//return;

	auto objects = root_node_.Query(object->GetAABB());
	auto collided_objects = object->GetCollidedObjects();
	for(std::set<std::shared_ptr<swganh::object::Object>>::iterator iter = collided_objects.begin(); iter != collided_objects.end();) {
		auto& other = *iter;
		if(iter == objects.end())
		{
			//std::cout << "Object::OnCollisionLeave "; std::wcout << "] " << object->GetCustomName() << "] "; std::cout << object->GetObjectId() << " (" << object->GetTemplate() << ") <-> "; std::wcout << other->GetCustomName() << "]"; std::cout << other->GetObjectId() << " (" << other->GetTemplate() << ")" << std::endl;
			object->RemoveCollidedObject(other);
			other->RemoveCollidedObject(object);

			object->OnCollisionLeave(other);
			other->OnCollisionLeave(object);
			
			iter = objects.erase(iter);
			continue;
		}
		else
		{
			// Make sure we still are intersecting.
			if(boost::geometry::intersects(object->GetWorldCollisionBox(), other->GetWorldCollisionBox()) == false) {
				//std::cout << "Object::OnCollisionLeave "; std::wcout << "] " << object->GetCustomName() << "] "; std::cout << object->GetObjectId() << " (" << object->GetTemplate() << ") <-> "; std::wcout << other->GetCustomName() << "]"; std::cout << other->GetObjectId() << " (" << other->GetTemplate() << ")" << std::endl;
				
				object->RemoveCollidedObject(other);
				other->RemoveCollidedObject(object);
				
				object->OnCollisionLeave(other);
				other->OnCollisionLeave(object);
				
				iter = objects.erase(iter);
				continue;
			}
		}
		iter++;
	}

	std::for_each(objects.begin(), objects.end(), [=](const std::shared_ptr<swganh::object::Object> other) {
		if(other->GetObjectId() == object->GetObjectId())
			return;

		if(boost::geometry::intersects(object->GetWorldCollisionBox(), other->GetWorldCollisionBox()))
		{
			bool found = false;
			auto collided_objects = object->GetCollidedObjects();
			std::for_each(collided_objects.begin(), collided_objects.end(), [=, &found](std::shared_ptr<Object> collided_object){
				if(collided_object->GetObjectId() == other->GetObjectId())
				{
					//std::cout << "Object::OnCollisionStay "; std::wcout << "] " << object->GetCustomName() << "] "; std::cout << object->GetObjectId() << " (" << object->GetTemplate() << ") <-> "; std::wcout << other->GetCustomName() << "]"; std::cout << other->GetObjectId() << " (" << other->GetTemplate() << ")" << std::endl;
					object->OnCollisionStay(other);
					other->OnCollisionStay(object);
					found = true;
				}
			});

			if(!found)
			{
				//std::cout << "Object::OnCollisionEnter "; std::wcout << "] " << object->GetCustomName() << "] "; std::cout << object->GetObjectId() << " (" << object->GetTemplate() << ") <-> "; std::wcout << other->GetCustomName() << "]"; std::cout << other->GetObjectId() << " (" << other->GetTemplate() << ")" << std::endl;
				
				object->AddCollidedObject(other);
				other->AddCollidedObject(object);

				object->OnCollisionEnter(other);
				other->OnCollisionEnter(object);
			}
		}
	});
}