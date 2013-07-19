// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include "swganh/tre/visitors/visitor_interface.h"

#include <map>
#include <memory>
#include <stack>
#include <vector>

#include "detail/header.h"
#include "detail/fractal.h"
#include "detail/layer.h"
#include "detail/container_layer.h"

namespace swganh
{
namespace tre
{
	class Fractal;
	class Layer;
	class ContainerLayer;
	
	class TerrainVisitor : public VisitorInterface
	{
	public:
		static const VisitorType Type = TRN_VISITOR;

		TerrainVisitor();
		~TerrainVisitor();

		/**
			@brief interprets a IFF::FileNode associated with this visitor.
			This should only be called by the IFFFile code.
		*/
		virtual void visit_data(uint32_t depth, std::string name, uint32_t size, swganh::ByteBuffer& data);

		/**
			@brief interprets a IFF::FolderNode associated with this visitor.
			This should only be called by the IFFFile code.
		*/
		virtual void visit_folder(uint32_t depth, std::string name, uint32_t size);

		const TrnHeader& GetHeader() { return header; }
		FractalMap& GetFractals() { return fractals_; }
		std::vector<std::shared_ptr<ContainerLayer>>& GetLayers() { return layers_; }

		float GetWaterHeight(float x, float z);
		float GetHeight(float x, float z);
		bool IsWater(float x, float z);

	private:
		float processLayerHeight(ContainerLayer* layer, float x, float z, float& base_value, float affector_transform, FractalMap& fractals);
		float calculateFeathering(float value, int featheringType);
		bool waterHeightHelper(ContainerLayer* layer, float x, float z, float& result);

		//This is working data.
		Fractal* working_fractal_;
		std::stack<std::pair<std::shared_ptr<Layer>, uint32_t>> layer_stack_;
		Layer* working_layer_;

		//This is actual persistance data
		TrnHeader header;
		std::map<uint32_t, std::unique_ptr<Fractal>> fractals_;
		std::vector<std::shared_ptr<ContainerLayer>> layers_;
	};
}
}
