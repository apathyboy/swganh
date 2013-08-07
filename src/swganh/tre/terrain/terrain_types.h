
#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "swganh/byte_buffer.h"
#include "swganh/tre/iff/iff.h"

namespace swganh {
namespace tre {
namespace detail_terrain {
	
	struct base_terrain_type
	{
		base_terrain_type()
			: node(nullptr) {}

		virtual ~base_terrain_type() {}

		virtual void deserialize(ByteBuffer& buffer) = 0;
		virtual void serialize(ByteBuffer& buffer) = 0;

		void load(iff_node* input)
		{
			node = input;
			deserialize(node->data);
		}

		void save()
		{
			if (node)
			{
				node->data.clear();
				serialize(node->data);
			}
		}

		bool is_loaded() const { return node != nullptr; }

		iff_node* node;
	};


	struct header : public base_terrain_type
	{
		std::string filename;
		float map_width;
		float chunk_width;
		uint32_t tiles_per_chunk;
		uint32_t use_global_water_height;
		float global_water_height;
		float water_shader_size;
		std::string water_shader_name;
		float seconds_per_world_cycle;

		float collidable_min_distance;
		float collidable_max_distance;
		float collidable_tile_size;
		float collidable_tile_border;
		float collidable_seed;

		float non_collidable_min_distance;
		float non_collidable_max_distance;
		float non_collidable_tile_size;
		float non_collidable_tile_border;
		float non_collidable_seed;

		float near_radial_min_distance;
		float near_radial_max_distance;
		float near_radial_tile_size;
		float near_radial_tile_border;
		float near_radial_seed;

		float far_radial_min_distance;
		float far_radial_max_distance;
		float far_radial_tile_size;
		float far_radial_tile_border;
		float far_radial_seed;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct footer : public base_terrain_type
	{
		float map_size;
		float chunk_width;
		uint32_t map_width;
		uint32_t map_height;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct smap : public base_terrain_type
	{
		std::vector<uint8_t> data;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct wmap : public base_terrain_type
	{
		std::vector<uint8_t> data;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct shader_family : public base_terrain_type
	{
		struct shader_child
		{
			std::string name;
			float weight;
		};

		uint32_t family_id;
		std::string family_name;
		std::string surface_properties_file;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		float unknown1;
		float feather_clamp;

		std::vector<std::unique_ptr<shader_child>> children;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct flora_family : public base_terrain_type
	{
		struct flora_child
		{
			std::string name;
			float weight;
			bool align_to_terrain;
			float displacement;
			float period;
			bool sway_flora;
			bool scale_flora;
			float min_scale;
			float max_scale;
		};

		uint32_t family_id;
		std::string family_name;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		float density;
		bool floats_on_water;

		std::vector<std::unique_ptr<flora_child>> children;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct radial_family : public base_terrain_type
	{
		struct radial_child
		{
			std::string name;
			float weight;
			float distance;
			float width;
			float height;
			bool maintain_aspect_ration;
			float displacement;
			float period;
			bool sway_flora;
			int unk9;
		};

		uint32_t family_id;
		std::string family_name;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		float density;

		std::vector<std::unique_ptr<radial_child>> children;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct environment_family : public base_terrain_type
	{
		uint32_t family_id;
		std::string family_name;
		uint8_t r;
		uint8_t g;
		uint8_t b;
		float feather_clamp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

}}}
