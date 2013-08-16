
#pragma once

#include <string>
#include <vector>

#include "terrain_types.h"

namespace swganh {
namespace terrain {

	// Make procedural_terrain's interface focused 100% on working with procedural terrain data.
	//
	// Iff format should be an implementation detail, imagine how multiple storage formats would be utilized(ie., json, xml, etc)
	struct procedural_terrain
	{
		header header;
		footer footer;

		wmap water_map;
		smap slope_map;

		terrain_group<shader_family> shader_group;
		terrain_group<flora_family> flora_group;
		terrain_group<radial_family> radial_group;
		terrain_group<environment_family> environment_group;
		terrain_group<fractal_family> fractal_group;
		std::vector<std::unique_ptr<construction_layer>> layers;
	};

	std::unique_ptr<procedural_terrain> read_procedural_terrain(ByteBuffer& buffer);
	ByteBuffer write_procedural_terrain(procedural_terrain& terrain);

}}
