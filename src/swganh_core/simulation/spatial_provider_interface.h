// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <list>
#include <memory>
#include <glm/glm.hpp>

#include "swganh_core/object/container_interface.h"
#include "swganh_core/object/object.h"


namespace swganh {
namespace simulation {

class WorldContainer;

/**
 * Provides Spatial Indexing functionality.
 */
class SpatialProviderInterface
{
public:
	virtual void SvgToFile()=0;
	virtual void AddObject(std::shared_ptr<swganh::object::Object> newObject, int32_t arrangement_id=-2)=0;
	virtual void RemoveObject(std::shared_ptr<swganh::object::Object> oldObject)=0;
	virtual void UpdateObject(std::shared_ptr<swganh::object::Object> obj, const swganh::object::AABB& old_bounding_volume, const swganh::object::AABB& new_bounding_volume, std::shared_ptr<swganh::object::Object> view_box, const swganh::object::AABB view_box_old_bounding_volume, const swganh::object::AABB view_box_new_bounding_volume) = 0;
	virtual void InsertObject(std::shared_ptr<swganh::object::Object> object)=0;
	virtual void DeleteObject(std::shared_ptr<swganh::object::Object> object)=0;
	virtual swganh::object::Object::ObjectPtrSet Query(boost::geometry::model::polygon<swganh::object::Point> query_box) = 0;
	virtual swganh::object::Object::ObjectPtrSet Query(glm::vec3 position = glm::vec3(), float radius = 0.0f) = 0;
	virtual std::set<std::pair<float, std::shared_ptr<swganh::object::Object>>> FindObjectsInRangeByTag(const std::shared_ptr<swganh::object::Object> requester, const std::string& tag, float range=-1)=0;
	virtual std::shared_ptr<WorldContainer>& GetWorldContainer() = 0;
};

}} // namespace swganh::simulation
