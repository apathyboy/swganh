
#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <glm/glm.hpp>

#include "swganh/utilities.h"
#include "swganh/byte_buffer.h"

namespace swganh {
namespace tre {
namespace detail_terrain {

	namespace detail {
#ifdef WIN32
		using std::make_unique;
#else
		using swganh::make_unique;
#endif
	}

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

	enum class e_water_type : int32_t
	{
		invalid = -1,
		water = 0,
		lava,
		COUNT
	};

	enum class e_affector_type : uint32_t
	{
		height_terrace = 0,
		height_constant,
		height_fractal,
		color_constant,
		color_ramp_height,
		color_ramp_fractal,
		shader_constant,
		shader_replace,
		flora_static_collidable_constant,
		flora_static_non_collidable_constant,
		flora_dynamic_near_constant,
		flora_dynamic_far_constant,
		exclude,
		passable, // missing example
		road,
		river,
		environment,
		ribbon, // missing example
		COUNT
	};

	enum class e_boundary_type : uint32_t
	{
		circle = 0,
		rectangle,
		polygon,
		polyline,
		COUNT
	};

	enum class e_filter_type : uint32_t
	{
		height = 0,
		fractal,
		slope,
		direction,
		shader,
		bitmap,
		COUNT
	};

	enum class e_layer_type : uint32_t
	{
		construction = 0,
		affector,
		boundary,
		filter,
		COUNT
	};

	template<typename T>
	class terrain_group
	{
	public:
		typedef T family_type;

		family_type* add_family(std::string name)
		{
			family_type* family = nullptr;

			auto new_family = detail::make_unique<family_type>();
			new_family->family_name = name;
			new_family->family_id = next_family_id();

			family = new_family.get();
			families_.push_back(std::move(new_family));

			return family;
		}

		void add_family(std::unique_ptr<family_type> family)
		{
			family_type* existing_family = find_family_by_id(family->family_id);
			if (existing_family)
			{
				throw std::runtime_error("family id already exists (" + std::to_string(existing_family->family_id) + std::string(") ") + existing_family->family_name);
			}

			families_.push_back(std::move(family));
		}

		void remove_family(uint32_t id)
		{
			auto remove_iter = std::remove_if(families_.begin(), families_.end(), [id](const std::unique_ptr<T>& family)
			{
				return family->id == id;
			});

			families_.erase(remove_iter, families_.end());
		}

		family_type* find_family_by_id(uint32_t id)
		{
			for (const auto& family : families_)
			{
				if (family->family_id == id)
				{
					return family.get();
				}
			}

			return nullptr;
		}

		family_type* find_family_by_name(const std::string& name)
		{
			for (const auto& family : families_)
			{
				if (family->family_name.compare(name) == 0)
				{
					return family.get();
				}
			}

			return nullptr;
		}

		const std::vector<std::unique_ptr<family_type>>& get_families() const { return families_; }

	private:
		uint32_t next_family_id() const
		{
			uint32_t last_id = 0;

			for (const auto& family : families_)
			{
				if (family->family_id > last_id)
				{
					last_id = family->family_id;
				}
			}

			return ++last_id;
		}

		std::vector<std::unique_ptr<family_type>> families_;
	};

	struct header
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

	struct footer
	{
		float map_size;
		float chunk_width;
		uint32_t map_width;
		uint32_t map_height;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct smap
	{
		std::vector<uint8_t> data;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct wmap
	{
		std::vector<uint8_t> data;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct shader_family
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

	struct flora_family
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

	struct radial_family
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

	struct environment_family
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

	struct fractal_family
	{
		struct fractal
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

	struct base_terrain_layer
	{
		bool enabled;
		std::string name;

		virtual e_layer_type get_layer_type() const = 0;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct base_affector_layer : public base_terrain_layer
	{
		e_layer_type get_layer_type() const override { return e_layer_type::affector; }

		virtual e_affector_type get_type() const = 0;
	};

	struct base_boundary_layer : public base_terrain_layer
	{
		e_layer_type get_layer_type() const override { return e_layer_type::boundary; }

		virtual e_boundary_type get_type() const = 0;
	};

	struct base_filter_layer : public base_terrain_layer
	{
		e_layer_type get_layer_type() const override { return e_layer_type::filter; }

		virtual e_filter_type get_type() const = 0;
	};

	struct construction_layer : public base_terrain_layer
	{
		e_layer_type get_layer_type() const override { return e_layer_type::construction; }

		bool invert_boundaries;
		bool invert_filters;
		uint32_t unknown1;
		std::string notes;

		std::vector<std::unique_ptr<base_affector_layer>> affectors;
		std::vector<std::unique_ptr<base_boundary_layer>> boundaries;
		std::vector<std::unique_ptr<base_filter_layer>> filters;
		std::vector<std::unique_ptr<construction_layer>> containers;

		std::vector<base_terrain_layer*> children;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_color_constant : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::color_constant;
		e_affector_type get_type() const { return affector_type; }

		operations operation;
		uint8_t r;
		uint8_t g;
		uint8_t b;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_color_ramp_fractal : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::color_ramp_fractal;
		e_affector_type get_type() const { return affector_type; }

		uint32_t family_id;
		operations operation;
		std::string ramp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_color_ramp_height : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::color_ramp_height;
		e_affector_type get_type() const { return affector_type; }

		operations operation;
		float low_height;
		float high_height;
		std::string ramp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_environment : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::environment;
		e_affector_type get_type() const { return affector_type; }

		uint32_t family_id;
		bool feather_clamp_override;
		float clamp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_exclude : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::exclude;
		e_affector_type get_type() const { return affector_type; }

		void deserialize(ByteBuffer& buffer) {}
		void serialize(ByteBuffer& buffer) {}
	};

	struct affector_flora_collidable_constant : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::flora_static_collidable_constant;
		e_affector_type get_type() const { return affector_type; }

		uint32_t adjust;
		e_flora_operation operation;
		bool remove_all_radial_flora;
		bool density_override;
		float density;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	// flora constant (collidable/non-collidable/near-radial)
	struct affector_flora_non_collidable_constant : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::flora_static_non_collidable_constant;
		e_affector_type get_type() const { return affector_type; }

		uint32_t adjust;
		e_flora_operation operation;
		bool remove_all_radial_flora;
		bool density_override;
		float density;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_height_constant : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::height_constant;
		e_affector_type get_type() const { return affector_type; }

		operations operation;
		float value;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_height_fractal : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::height_fractal;
		e_affector_type get_type() const { return affector_type; }

		uint32_t family_id;
		operations operation;
		float y_scale;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	// AHTR
	struct affector_height_terrace : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::height_terrace;
		e_affector_type get_type() const { return affector_type; }

		float terrace_height;
		float flat_ratio;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	// flora constant (collidable/non-collidable/near-radial)
	struct affector_radial_far_constant : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::flora_dynamic_far_constant;
		e_affector_type get_type() const { return affector_type; }

		uint32_t adjust;
		e_flora_operation operation;
		bool remove_all_radial_flora;
		bool density_override;
		float density;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_radial_near_constant : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::flora_dynamic_near_constant;
		e_affector_type get_type() const { return affector_type; }

		uint32_t adjust;
		e_flora_operation operation;
		bool remove_all_radial_flora;
		bool density_override;
		float density;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_river : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::river;
		e_affector_type get_type() const { return affector_type; }

		struct segment
		{
			struct control_point
			{
				float x_pos;
				float z_pos;
				float height;
			};

			std::vector<control_point> control_points;

			void deserialize(ByteBuffer& buffer);
			void serialize(ByteBuffer& buffer);
		};

		std::vector<std::unique_ptr<segment>> segments;
		std::vector<glm::vec2> control_points;
		float feathering_distance;
		e_feathering_function feathering_function;
		uint32_t bank_shader;
		uint32_t bottom_shader;
		float width;
		float trench_depth;
		float velocity;
		bool has_water;
		float water_depth;
		float water_width;
		float water_shader_size;
		std::string water_shader;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_road : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::road;
		e_affector_type get_type() const { return affector_type; }

		struct segment
		{
			struct control_point
			{
				float x_pos;
				float z_pos;
				float height;
			};

			std::vector<control_point> control_points;

			void deserialize(ByteBuffer& buffer);
			void serialize(ByteBuffer& buffer);
		};

		std::vector<std::unique_ptr<segment>> segments;
		std::vector<glm::vec2> control_points;
		float width;
		uint32_t shader;

		e_feathering_function height_feathering_function;
		float height_feathering_distance;

		e_feathering_function shader_feathering_function;
		float shader_feathering_distance;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_shader_constant : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::shader_constant;
		e_affector_type get_type() const { return affector_type; }

		uint32_t family_id;
		bool feather_clamp_override;
		float clamp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct affector_shader_replace : public base_affector_layer
	{
		static const e_affector_type affector_type = e_affector_type::shader_replace;
		e_affector_type get_type() const { return affector_type; }

		uint32_t source_family;
		uint32_t destination_family;
		bool feather_clamp_override;
		float clamp;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct boundary_circle : public base_boundary_layer
	{
		static const e_boundary_type boundary_type = e_boundary_type::circle;
		e_boundary_type get_type() const { return boundary_type; }

		float center_x;
		float center_z;
		float radius;
		e_feathering_function function;
		float distance;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct boundary_polygon : public base_boundary_layer
	{
		static const e_boundary_type boundary_type = e_boundary_type::polygon;
		e_boundary_type get_type() const { return boundary_type; }

		std::vector<glm::vec2> control_points;
		e_feathering_function function;
		float distance;
		bool is_local_water_table;
		float water_height;
		float water_shader_size;
		std::string water_shader;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct boundary_polyline : public base_boundary_layer
	{
		static const e_boundary_type boundary_type = e_boundary_type::polyline;
		e_boundary_type get_type() const { return boundary_type; }

		std::vector<glm::vec2> control_points;
		e_feathering_function function;
		float distance;
		float width;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct boundary_rectangle : public base_boundary_layer
	{
		static const e_boundary_type boundary_type = e_boundary_type::rectangle;
		e_boundary_type get_type() const { return boundary_type; }

		float x1;
		float z1;
		float x2;
		float z2;
		e_feathering_function function;
		float distance;
		bool is_local_water_table;
		bool local_global_water_table;
		float water_height;
		float water_shader_size;
		std::string water_shader;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct filter_direction : public base_filter_layer
	{
		static const e_filter_type filter_type = e_filter_type::direction;
		e_filter_type get_type() const { return filter_type; }

		float min_angle;
		float max_angle;
		e_feathering_function feathering;
		float feather_distance;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct filter_fractal : public base_filter_layer
	{
		static const e_filter_type filter_type = e_filter_type::fractal;
		e_filter_type get_type() const { return filter_type; }

		uint32_t family_id;
		e_feathering_function feathering;
		float feather_distance;
		float low;
		float high;
		float step;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct filter_height : public base_filter_layer
	{
		static const e_filter_type filter_type = e_filter_type::height;
		e_filter_type get_type() const { return filter_type; }

		float low_height;
		float high_height;
		e_feathering_function feathering;
		float feather_distance;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct filter_shader : public base_filter_layer
	{
		static const e_filter_type filter_type = e_filter_type::shader;
		e_filter_type get_type() const { return filter_type; }

		uint32_t family_id;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};

	struct filter_slope : public base_filter_layer
	{
		static const e_filter_type filter_type = e_filter_type::slope;
		e_filter_type get_type() const { return filter_type; }

		float min_angle;
		float max_angle;
		e_feathering_function feathering;
		float feather_distance;

		void deserialize(ByteBuffer& buffer);
		void serialize(ByteBuffer& buffer);
	};
}}}
