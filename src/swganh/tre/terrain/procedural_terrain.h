
#pragma once

#include <string>
#include <vector>

#include "terrain_types.h"

namespace swganh {
namespace tre {

	struct iff_node;
	struct procedural_terrain_impl;

	class procedural_terrain
	{
	public:
		explicit procedural_terrain(iff_node* head);
		~procedural_terrain();

		void load();
		void save();

		const iff_node& iff_doc() const;

		void set_filename(std::string filename);
		std::string get_filename() const;

	private:
		std::unique_ptr<procedural_terrain_impl> impl_;
	};

}}