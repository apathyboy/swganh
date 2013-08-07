
#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "swganh/byte_buffer.h"
#include "swganh/tre/iff/iff.h"

namespace swganh {
namespace tre {
namespace detail_terrain {
	
	enum class combination_rules : uint32_t
	{
		add = 0,
		multiply,
		crest,
		turbulence,
		crest_clamp,
		turbulence_clamp,
		COUNT
	};

	enum class operations : uint32_t
	{
		replace = 0,
		add,
		subtract,
		multiply,
		COUNT
	};

	enum class e_feathering_function : uint32_t
	{
		linear = 0,
		easeIn,
		easeOut,
		easeInOut,
		COUNT
	};

	enum class e_adjust_type : uint32_t
	{
		adjustBorder = 0,
		adjustOutside,
		COUNT
	};

	enum class e_flora_operation : uint32_t
	{
		add = 0,
		remove,
		COUNT
	};

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

	struct fractal_family : public base_terrain_type
	{
		struct fractal : public base_terrain_type
		{
			uint32_t seed;
			bool use_bias;
			float bias;
			bool use_gain;
			float gain;
			uint32_t unknown1;
			float octaves;
			float amplitude;
			float x_scale;
			float y_scale;
			float x_offset;
			float y_offset;
			combination_rules rule;

			void deserialize(ByteBuffer& buffer);
			void serialize(ByteBuffer& buffer);
		};

		uint32_t family_id;
		std::string family_name;

		std::unique_ptr<fractal> fractal_data;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct layer_header : public base_terrain_type
	{
		bool enabled;
		std::string name;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct base_terrain_layer : public base_terrain_type
	{
		std::unique_ptr<layer_header> header;

		base_terrain_layer* parent;
		std::vector<std::unique_ptr<base_terrain_layer>> children;
	};

	struct default_layer : public base_terrain_layer
	{
		std::vector<uint8_t> data;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_color_ramp_fractal : public base_terrain_layer
	{
		uint32_t family_id;
		operations operation;
		std::string ramp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_environment : public base_terrain_layer
	{
		uint32_t family_id;
		bool feather_clamp_override;
		float clamp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	// flora constant (collidable/non-collidable/near-radial)
	struct affector_flora_non_collidable_constant : public base_terrain_layer
	{
		uint32_t adjust;
		e_flora_operation operation;
		bool remove_all_radial_flora;
		bool density_override;
		float density;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_height_fractal : public base_terrain_layer
	{
		uint32_t family_id;
		operations operation;
		float y_scale;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_height_terrace : public base_terrain_layer
	{};

	// flora constant (collidable/non-collidable/near-radial)
	struct affector_radial_near_constant : public base_terrain_layer
	{
		uint32_t adjust;
		e_flora_operation operation;
		bool remove_all_radial_flora;
		bool density_override;
		float density;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_shader_constant : public base_terrain_layer
	{
		uint32_t family_id;
		bool feather_clamp_override;
		float clamp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};
	
	struct boundary_circle : public base_terrain_layer
	{};

	struct boundary_polygon : public base_terrain_layer
	{};

	struct construction_layer : public base_terrain_layer
	{
		bool invert_boundaries;
		bool invert_filters;
		uint32_t unknown1;
		std::string notes;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct filter_fractal : public base_terrain_layer
	{};

	struct filter_height : public base_terrain_layer
	{};

	struct filter_slope : public base_terrain_layer
	{
		float min_angle;
		float max_angle;
		e_feathering_function feathering;
		float feather_distance;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};
}}}
