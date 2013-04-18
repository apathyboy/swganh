// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "quadtree_spatial_provider.h"

#include "swganh/logger.h"

#include "swganh_core/object/object.h"
#include "swganh_core/object/permissions/world_permission.h"
#include "swganh_core/simulation/player_view_box.h"
#include "swganh_core/simulation/world_container.h"

using std::shared_ptr;

using namespace swganh::observer;
using namespace swganh::object;
using namespace swganh::simulation;
using namespace quadtree;

static int VIEWING_RANGE = 128;

QuadtreeSpatialProvider::QuadtreeSpatialProvider()
	: root_node_(ROOT, Region(quadtree::Point(-8300.0f, -8300.0f), 
	quadtree::Point(8300.0f, 8300.0f)), 0, 9, nullptr),
	world_container_(std::make_shared<WorldContainer>(this))
{
}

QuadtreeSpatialProvider::~QuadtreeSpatialProvider(void)
{
}

void QuadtreeSpatialProvider::AddObject(shared_ptr<Object> object, int32_t arrangement_id)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);
	
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

	root_node_.InsertObject(object);
	object->SetArrangementId(arrangement_id);
	object->SetSceneId(scene_id_);

	if(object->GetContainer() == nullptr)
		object->SetContainer(world_container_);

	// Add our children.
	object->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		child->SetSceneId(scene_id_);
		#ifdef SI_METHOD_ONE
		child->BuildSpatialProfile();
		child->UpdateWorldCollisionBox();
		child->UpdateAABB();
		root_node_.InsertObject(child);
		#endif
	});

	CheckCollisions(object);

	object->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		CheckCollisions(child);
	});

	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();

	std::cout << "SpatialProvider::AddObject Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() << "ms" << std::endl;

}

void QuadtreeSpatialProvider::InsertObject(shared_ptr<Object> object)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);
	
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

	root_node_.InsertObject(object);
	object->SetArrangementId(2);
	object->SetSceneId(scene_id_);
	object->SetContainer(world_container_);

	// Add our children.
	object->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		child->SetSceneId(scene_id_);
		#ifdef SI_METHOD_ONE
		root_node_.InsertObject(child);
		#endif
	});

	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();

	std::cout << "SpatialProvider::InsertObject Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() << "ms" << std::endl;
}

void QuadtreeSpatialProvider::DeleteObject(shared_ptr<Object> object)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

	#ifdef SI_METHOD_ONE
	// Remove our children.
	object->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		root_node_.RemoveObject(child);
	});
	#endif

	root_node_.RemoveObject(object);
	object->SetContainer(nullptr);

	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();

	std::cout << "SpatialProvider::DeleteObject Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() << "ms" << std::endl;
}

void QuadtreeSpatialProvider::RemoveObject(shared_ptr<Object> object)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);
	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();

	// Remove our children.
	object->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		auto collided_objects = child->GetCollidedObjects();

		#ifdef SI_METHOD_ONE
		root_node_.RemoveObject(child);
		#endif
	
		for(auto& collided_object : collided_objects)
		{
			object->OnCollisionLeave(collided_object);
			collided_object->OnCollisionLeave(object);

			object->RemoveCollidedObject(collided_object);
			collided_object->RemoveCollidedObject(object);
		}
	});

	auto collided_objects = object->GetCollidedObjects(); // Copy
	root_node_.RemoveObject(object);

	// Manually trigger leave.
	for(auto& collided_object : collided_objects)
	{
		collided_object->OnCollisionLeave(object);
		object->OnCollisionLeave(collided_object);

		collided_object->RemoveCollidedObject(object);
		object->RemoveCollidedObject(collided_object);
	}

	object->SetContainer(nullptr);


	std::chrono::high_resolution_clock::time_point stop_time = std::chrono::high_resolution_clock::now();

	std::cout << "SpatialProvider::RemoveObject Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() << "ms" << std::endl;
}

void QuadtreeSpatialProvider::UpdateObject(shared_ptr<Object> obj, const swganh::object::AABB& old_bounding_volume, const swganh::object::AABB& new_bounding_volume, std::shared_ptr<swganh::object::Object> view_box,
										   const swganh::object::AABB view_box_old_bounding_volume, const swganh::object::AABB view_box_new_bounding_volume)
{
	boost::upgrade_lock<boost::shared_mutex> uplock(lock_);

	std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
	root_node_.UpdateObject(obj, old_bounding_volume, new_bounding_volume);
	
	// Update children
	obj->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		auto child_old_aabb = child->GetAABB();
		child->UpdateWorldCollisionBox();
		child->UpdateAABB();
		#ifdef SI_METHOD_ONE
		root_node_.UpdateObject(child, child_old_aabb, child->GetAABB());
		#endif
	});

	if(view_box != nullptr) {
		root_node_.UpdateObject(view_box, view_box_old_bounding_volume, view_box_new_bounding_volume);
		CheckCollisions(view_box);
	}

	CheckCollisions(obj);

	obj->ViewObjects(nullptr, 0, true, [=](std::shared_ptr<swganh::object::Object> child) {
		CheckCollisions(child);
	});

	auto stop_time = std::chrono::high_resolution_clock::now();

    std::cout << "SpatialProvider::UpdateObject Duration: " << std::chrono::duration_cast<std::chrono::milliseconds>(stop_time - start_time).count() << "ms" << std::endl;
}

QueryBox QuadtreeSpatialProvider::GetQueryBoxViewRange(std::shared_ptr<Object> object)
{
	glm::vec3 position;
	glm::quat orientation;
	object->__InternalGetAbsolutes(position, orientation);
	return QueryBox(quadtree::Point(position.x - VIEWING_RANGE, position.z - VIEWING_RANGE), 
					quadtree::Point(position.x + VIEWING_RANGE, position.z + VIEWING_RANGE));	
}

swganh::object::Object::ObjectPtrSet QuadtreeSpatialProvider::Query(boost::geometry::model::polygon<swganh::object::Point> query_box)
{
	swganh::object::Object::ObjectPtrSet return_vector;
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

swganh::object::Object::ObjectPtrSet QuadtreeSpatialProvider::Query(glm::vec3 position, float radius)
{
	swganh::object::Object::ObjectPtrSet return_vector;
	
	AABB aabb;
	QueryBox query_box;

	if(radius <= 0)
		query_box = QueryBox(quadtree::Point(1.0f - radius, 1.0f - radius), 
					quadtree::Point(1.0f + radius, 1.0f + radius));
	else
		query_box = QueryBox(quadtree::Point(position.x - radius, position.z - radius), 
					quadtree::Point(position.x + radius, position.z + radius));

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
	auto abs_position = glm::vec3();
	auto abs_orientation = glm::quat();

	requester->GetAbsolutes(abs_position, abs_orientation);
	auto objects = Query(abs_position, range);
	for(auto& object : objects)
	{
		if(object->HasFlag(tag))
		{
			glm::vec3 pos1, pos2;
			glm::quat o1, o2;
			requester->GetAbsolutes(pos1, o1);
			object->GetAbsolutes(pos2, o2);

			obj_map.insert(std::pair<float, std::shared_ptr<swganh::object::Object>>(glm::distance(pos1, pos2),object));
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
	for(swganh::object::Object::ObjectPtrSet::iterator iter = collided_objects.begin(); iter != collided_objects.end();) {
		auto& other = *iter;

		// If we aren't still intersecting, remove.
		if(boost::geometry::intersects(object->GetWorldCollisionBox(), other->GetWorldCollisionBox()) == false) {
			//std::cout << "Object::OnCollisionLeave "; std::wcout << "] " << object->GetCustomName() << "] "; std::cout << object->GetObjectId() << " (" << object->GetTemplate() << ") <-> "; std::wcout << other->GetCustomName() << "]"; std::cout << other->GetObjectId() << " (" << other->GetTemplate() << ")" << std::endl;
				
			object->RemoveCollidedObject(other);
			other->RemoveCollidedObject(object);
				
			object->OnCollisionLeave(other);
			other->OnCollisionLeave(object);
				
			//iter = objects.erase(iter);
			iter++;
			continue;
		}

		iter++;
	}

	std::for_each(objects.begin(), objects.end(), [=](const std::shared_ptr<swganh::object::Object> other) {
		if(other->GetObjectId() == object->GetObjectId())
			return;

		//std::cout << "Checking intersect for " << object->GetTemplate() << " <-> " << other->GetTemplate() << std::endl;
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
				//std::cout << "Object::OnCollisionEnter "; std::wcout << " [" << object->GetCustomName() << "] "; std::cout << object->GetObjectId() << " (" << object->GetTemplate() << ") <-> ["; std::wcout << other->GetCustomName() << "]"; std::cout << other->GetObjectId() << " (" << other->GetTemplate() << ")" << std::endl;
				
				object->AddCollidedObject(other);
				other->AddCollidedObject(object);

				object->OnCollisionEnter(other);
				other->OnCollisionEnter(object);
			}
		}
	});
}
