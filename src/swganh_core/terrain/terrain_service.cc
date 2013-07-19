// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "terrain_service.h"

#include <map>

#include "swganh_core/simulation/scene_events.h"

#include <swganh/event_dispatcher.h>
#include <swganh/logger.h>

#include "swganh/tre/resource_manager.h"
//#include "swganh/tre/visitors/terrain/layer_visitor.h"
#include "swganh/tre/visitors/terrain/terrain_visitor.h"

#include "swganh/tre/visitors/terrain/detail/container_layer.h"
#include "swganh/tre/visitors/terrain/detail/boundary_layer.h"
#include "swganh/tre/visitors/terrain/detail/height_layer.h"
#include "swganh/tre/visitors/terrain/detail/filter_layer.h"
#include "swganh/tre/visitors/terrain/detail/boundary_polygon.h"
#include "swganh/tre/visitors/terrain/detail/header.h"

using namespace swganh::terrain;
using namespace swganh::tre;

TerrainService::TerrainService(swganh::app::SwganhKernel* kernel) : kernel_(kernel)
{
	kernel_->GetEventDispatcher()->Subscribe("SceneManager:NewScene", [&] (const std::shared_ptr<swganh::EventInterface>& newEvent)
	{
		auto real_event = std::static_pointer_cast<swganh::simulation::NewSceneEvent>(newEvent);
		try
		{
			SceneEntry entry;
			entry.terrain_visitor_ = kernel_->GetResourceManager()->GetResourceByName<TerrainVisitor>(real_event->terrain_filename, false);
			
			boost::lock_guard<boost::mutex> lock(terrain_mutex_);
			scenes_.insert(SceneMap::value_type(real_event->scene_id, std::move(entry)));
		}
		catch(...)
		{
			LOG(error) << "Failed to load trn file: " << real_event->terrain_filename;
		}
	});

	kernel_->GetEventDispatcher()->Subscribe("SceneManager:DestroyScene", [&] (const std::shared_ptr<swganh::EventInterface>& newEvent)
	{
		auto real_event = std::static_pointer_cast<swganh::simulation::DestroySceneEvent>(newEvent);
		
		boost::lock_guard<boost::mutex> lock(terrain_mutex_);
		this->scenes_.erase(real_event->scene_id);
	});
    
    SetServiceDescription(swganh::service::ServiceDescription(        
		"Terrain Service",
        "terrain",
        "0.1",
        "127.0.0.1", 
        0,
        0, 
        0));
}

TerrainService::~TerrainService()
{}

void TerrainService::Initialize()
{}

float TerrainService::GetWaterHeight(uint32_t scene_id, float x, float z, float raw)
{
	boost::lock_guard<boost::mutex> lock(terrain_mutex_);
	auto itr = scenes_.find(scene_id);
	if(itr != scenes_.end())
	{
		return itr->second.terrain_visitor_->GetHeight(x, z);
	}
	return FLT_MIN;
}

float TerrainService::GetHeight(uint32_t scene_id, float x, float z, bool raw)
{
	boost::lock_guard<boost::mutex> lock(terrain_mutex_);
	auto itr = scenes_.find(scene_id);
	if(itr != scenes_.end())
	{
		//Read in height at this point
		return itr->second.terrain_visitor_->GetHeight(x, z);
	}
	return FLT_MIN;
}

bool TerrainService::IsWater(uint32_t scene_id, float x, float z, bool raw)
{
	float water_height = GetWaterHeight(scene_id, x, z, raw);
	if (water_height != FLT_MIN)
	{
		float height = GetHeight(scene_id, x, z);
		if (height <= water_height)
			return true;
	}
	return false;
}
