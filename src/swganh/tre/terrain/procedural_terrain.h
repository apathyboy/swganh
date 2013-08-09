
#pragma once

#include <string>
#include <vector>

#include "terrain_types.h"

namespace swganh {
namespace tre {

	struct iff_node;
	struct procedural_terrain_impl;


	// Make procedural_terrain's interface focused 100% on working with procedural terrain data.
	// 
	// Iff format should be an implementation detail, imagine how multiple storage formats would be utilized(ie., json, xml, etc)
	class procedural_terrain
	{
	public:
		explicit procedural_terrain(ByteBuffer& buffer);

		~procedural_terrain();

		void load();
		void save();

		iff_node* iff_doc() const;

		detail_terrain::terrain_group<detail_terrain::shader_family>& shader_group();
		detail_terrain::terrain_group<detail_terrain::flora_family>& flora_group();
		detail_terrain::terrain_group<detail_terrain::radial_family>& radial_group();
		detail_terrain::terrain_group<detail_terrain::environment_family>& environment_group();
		detail_terrain::terrain_group<detail_terrain::fractal_family>& fractal_group();

		std::vector<std::unique_ptr<detail_terrain::base_terrain_layer>>& layers();

	private:

		std::unique_ptr<procedural_terrain_impl> impl_;
	};

}}