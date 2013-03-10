// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include "swganh_core/simulation/spatial_provider_interface.h"
#include "swganh_core/object/permissions/container_permissions_interface.h"
#include "node.h"

namespace swganh {
namespace simulation {

class QuadtreeSpatialProvider 
	: public swganh::simulation::SpatialProviderInterface
{
public:
	QuadtreeSpatialProvider();
	virtual ~QuadtreeSpatialProvider(void);

	uint64_t GetObjectId() { return 0; }

	void SvgToFile() { 
		std::stringstream fname;
		fname << "./logs/scene_graph_" <<  scene_name_ << ".svg";
		root_node_.SvgDump(fname.str()); 
	}

	void SetSceneName(std::string name) { scene_name_ = name; }
	void SetSceneId(uint32_t id) { scene_id_ = id; }

	//Object Management
	virtual void AddObject(std::shared_ptr<swganh::object::Object> newObject, int32_t arrangement_id=-2);
	virtual void RemoveObject(std::shared_ptr<swganh::object::Object> oldObject);
	virtual void UpdateObject(std::shared_ptr<swganh::object::Object> obj, const swganh::object::AABB& old_bounding_volume, const swganh::object::AABB& new_bounding_volume, std::shared_ptr<swganh::object::Object> view_box = nullptr, const swganh::object::AABB view_box_old_bounding_volume = swganh::object::AABB(), const swganh::object::AABB view_box_new_bounding_volume = swganh::object::AABB());
	virtual std::set<std::shared_ptr<swganh::object::Object>> Query(boost::geometry::model::polygon<swganh::object::Point> query_box);
	virtual std::set<std::shared_ptr<swganh::object::Object>> Query(glm::vec3 position = glm::vec3(), float range = 0.0f);

	virtual std::set<std::pair<float, std::shared_ptr<swganh::object::Object>>> FindObjectsInRangeByTag(const std::shared_ptr<swganh::object::Object> requester, const std::string& tag, float range=-1);


private:
	quadtree::Node root_node_;

	std::string scene_name_;
	uint32_t scene_id_;

	boost::shared_mutex lock_;

	void CheckCollisions(std::shared_ptr<swganh::object::Object> object);
};

}} // swganh::simulation
