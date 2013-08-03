// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#include "terrain_visitor.h"

#include "detail/layer_loader.h"

#include "swganh/logger.h"

#include "detail/container_layer.h"
#include "detail/boundary_layer.h"
#include "detail/height_layer.h"
#include "detail/filter_layer.h"
#include "detail/boundary_polygon.h"
#include "detail/header.h"


using namespace swganh::tre;

TerrainVisitor::TerrainVisitor()
{}

TerrainVisitor::~TerrainVisitor()
{}

void TerrainVisitor::visit_data(uint32_t depth, std::string name, uint32_t size, swganh::ByteBuffer& data)
{
	if (name == "0014DATA")
	{
		//Loading header data
		header.Deserialize(data);
	}
	else if(name == "MFAMDATA")
	{
		//Reading a new fractal
		//working_fractal_ = new Fractal(data);
		auto tmp = std::make_unique<Fractal>(data);
		working_fractal_ = tmp.get();
		fractals_.insert(std::make_pair(tmp->fractal_id, std::move(tmp)));
	} 
	else if(name == "0001DATA" && working_fractal_ != nullptr) 
	{
		//Filling in data for a fractal
		working_fractal_->Deserialize(data);
		working_fractal_ = nullptr;
	}
	else if(working_layer_ != nullptr && name == "0001DATA")
	{
		//Loading basic layer data for the layer
		working_layer_->SetData(data);
	}
	else if(working_layer_ != nullptr && (name == "DATAPARM" || name == "DATA" || name == "ADTA"))
	{
		//Loading in layer specific layer data
		working_layer_->Deserialize(data);
		working_layer_ = nullptr;
	}
	else if (name == "WMAP")
	{
		wmap_.insert(wmap_.begin(), data.data(), data.data() + size);
	}
	else if (name == "SMAP")
	{
		smap_.insert(smap_.begin(), data.data(), data.data() + size);
	}
	else if (name == "0001DATA")
	{
		//Loading basic layer data for the layer
		footer.Deserialize(data);
	}
}

void TerrainVisitor::visit_folder(uint32_t depth, std::string name, uint32_t size)
{	
	//Get our layer stack back to where it should be
	while(layer_stack_.size() > 0 && layer_stack_.top().second > depth)
	{
		layer_stack_.pop();
	}
	
	//If we can create the layer, we have an implementation for it
	auto test_layer_ = LayerLoader(name);
	if(test_layer_ != nullptr)
	{
		//We created a layer, so set it as the working layer
		working_layer_ = test_layer_.get();
	
		//Hook the layer into either the top level layer list, or it's parent
		if(layer_stack_.size() == 0 && working_layer_->GetType() == LAYER_TYPE_CONTAINER)
		{
			layers_.push_back(std::static_pointer_cast<ContainerLayer>(test_layer_));
		} 
		else if(layer_stack_.top().first->GetType() == LAYER_TYPE_CONTAINER)
		{
			std::static_pointer_cast<ContainerLayer>(layer_stack_.top().first)
				->InsertLayer(test_layer_);
		}
	
		//Add the layer to the stack
		layer_stack_.push(std::make_pair(test_layer_, depth));
	}
}

float TerrainVisitor::GetWaterHeight(float x, float z)
{
	for (auto& child : GetLayers())
	{
		float result;
		if (waterHeightHelper(child.get(), x, z, result))
		{
			return result;
		}
	}

	//Todo:Apply any necessary layer modifications

	if (header.use_global_water_height)
	{
		return header.global_water_height;
	}

	return FLT_MIN;
}

float TerrainVisitor::GetHeight(float x, float z)
{
	//Read in height at this point
	auto& fractals = GetFractals();

	float affector_transform = 1.0f;
	float height_result = 0.0f;

	for (auto& layer : GetLayers())
	{
		if (layer->enabled)
		{
			processLayerHeight(layer.get(), x, z, height_result, affector_transform, fractals);
		}
	}

	//Todo:Apply any necessary layer modifications

	return (float) height_result;
}

bool TerrainVisitor::IsWater(float x, float z)
{
	float water_height = GetWaterHeight(x, z);
	if (water_height != FLT_MIN)
	{
		float height = GetHeight(x, z);
		if (height <= water_height)
			return true;
	}
	return false;
}

float TerrainVisitor::processLayerHeight(ContainerLayer* layer, float x, float z, float& base_value, float affector_transform, FractalMap& fractals)
{
	auto boundaries = layer->boundaries;
	auto heights = layer->heights;
	auto filters = layer->filters;

	float transform_value = 0.0f;
	bool has_boundaries = false;

	for (unsigned int i = 0; i < boundaries.size(); i++)
	{
		BoundaryLayer* boundary = (BoundaryLayer*) boundaries.at(i).get();

		if (!boundary->enabled)
			continue;
		else
			has_boundaries = true;

		float result = (float) boundary->Process(x, z);

		result = calculateFeathering(result, boundary->feather_type);

		if (result > transform_value)
			transform_value = result;

		if (transform_value >= 1)
			break;
	}

	if (has_boundaries == false)
		transform_value = 1.0f;

	if (layer->invert_boundaries)
		transform_value = 1.0f - transform_value;

	if (transform_value != 0)
	{
		for (unsigned int i = 0; i < filters.size(); ++i)
		{
			FilterLayer* filter = (FilterLayer*) filters.at(i).get();

			if (!filter->enabled)
				continue;

			float result = (float) filter->Process(x, z, transform_value, base_value, fractals);

			result = calculateFeathering(result, filter->feather_type);

			if (transform_value > result)
				transform_value = result;

			if (transform_value == 0)
				break;
		}

		if (layer->invert_filters)
			transform_value = 1.0f - transform_value;

		if (transform_value != 0)
		{
			for (unsigned int i = 0; i < heights.size(); i++)
			{
				HeightLayer* affector = (HeightLayer*) heights.at(i).get();

				if (affector->enabled)
				{
					affector->GetBaseHeight(x, z, transform_value, base_value, fractals);
				}
			}

			for (const auto& child : layer->children)
			{
				if (child->enabled)
					processLayerHeight(child.get(), x, z, base_value, affector_transform * transform_value, fractals);
			}
		}
	}

	return transform_value;
}

float TerrainVisitor::calculateFeathering(float value, int featheringType)
{
	float result = value;

	switch (featheringType) {
	case 1:
		result = result * result;
		break;
	case 2:
		result = sqrt(result);
		break;
	case 3:
		result = result * result * (3 - 2 * result);
		break;
	case 0:
		break;
	default:
		result = 0;
		break;
	}

	return result;
}

bool TerrainVisitor::waterHeightHelper(ContainerLayer* layer, float x, float z, float& result)
{
	//Check our boundaries
	for (auto& boundary : layer->boundaries)
	{
		if (boundary->GetType() == LAYER_TYPE_BOUNDARY_POLYGON)
		{
			BoundaryPolygon* bpol = (BoundaryPolygon*) boundary.get();
			if (bpol->use_water_height && bpol->IsContained(x, z))
			{
				result = bpol->water_height;
				return true;
			}
		}
	}

	//Check our children recursively
	for (auto& child : layer->children)
	{
		if (waterHeightHelper(child.get(), x, z, result))
		{
			return true;
		}
	}

	return false;
}
