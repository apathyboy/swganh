
#pragma once

#include <string>
#include <vector>

#include "terrain_types.h"

namespace swganh {
namespace tre {

	// Make procedural_terrain's interface focused 100% on working with procedural terrain data.
	// 
	// Iff format should be an implementation detail, imagine how multiple storage formats would be utilized(ie., json, xml, etc)
	struct procedural_terrain
	{
		detail_terrain::header header;
		detail_terrain::footer footer;

		detail_terrain::wmap water_map;
		detail_terrain::smap slope_map;

		detail_terrain::terrain_group<detail_terrain::shader_family> shader_group;
		detail_terrain::terrain_group<detail_terrain::flora_family> flora_group;
		detail_terrain::terrain_group<detail_terrain::radial_family> radial_group;
		detail_terrain::terrain_group<detail_terrain::environment_family> environment_group;
		detail_terrain::terrain_group<detail_terrain::fractal_family> fractal_group;
		std::vector<std::unique_ptr<detail_terrain::construction_layer>> layers;
	};

	std::unique_ptr<procedural_terrain> read_procedural_terrain(ByteBuffer& buffer);
	ByteBuffer write_procedural_terrain(procedural_terrain& terrain);

}}