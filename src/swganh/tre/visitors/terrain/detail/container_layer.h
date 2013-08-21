// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#pragma once

#include <vector>

#include "layer.h"
#include "boundary_layer.h"
#include "height_layer.h"
#include "filter_layer.h"

namespace swganh
{
namespace tre
{
	class BoundaryLayer;
	class FilterLayer;
	class HeightLayer;

	class ContainerLayer : public Layer
	{
	public:
        
        virtual ~ContainerLayer() {}

		virtual void Deserialize(swganh::ByteBuffer& buffer)
		{
			//Read ADTA
			invert_boundaries = buffer.read<uint32_t>() == 1;
			invert_filters = buffer.read<uint32_t>() == 1;
			buffer.read<uint32_t>();
			notes = buffer.read<std::string>(true);
		}

		virtual LayerType GetType() { return LAYER_TYPE_CONTAINER; }

		void InsertLayer(std::shared_ptr<Layer> layer)
		{
			switch(layer->GetType())
			{
			case LAYER_TYPE_CONTAINER:
				children.push_back(std::static_pointer_cast<ContainerLayer>(layer));
				break;
			case LAYER_TYPE_BOUNDARY:
			case LAYER_TYPE_BOUNDARY_POLYGON:
				boundaries.push_back(std::static_pointer_cast<BoundaryLayer>(layer));
				break;
			case LAYER_TYPE_HEIGHT:
				heights.push_back(std::static_pointer_cast<HeightLayer>(layer));
				break;
			case LAYER_TYPE_FILTER:
				filters.push_back(std::static_pointer_cast<FilterLayer>(layer));
			}
		}

		std::vector<std::shared_ptr<ContainerLayer>> children;
		std::vector<std::shared_ptr<BoundaryLayer>> boundaries;
		std::vector<std::shared_ptr<FilterLayer>> filters;
		std::vector<std::shared_ptr<HeightLayer>> heights;

		bool invert_boundaries, invert_filters;
		std::string notes;
	private:
	};

}
}
