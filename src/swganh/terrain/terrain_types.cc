
#include "terrain_types.h"

using namespace swganh::terrain;
using swganh::ByteBuffer;

void header::deserialize(ByteBuffer& buffer)
{
	filename = buffer.read<std::string>(false, true);
	map_width = buffer.read<float>();
	chunk_width = buffer.read<float>();
	tiles_per_chunk = buffer.read<uint32_t>();
	use_global_water_height = buffer.read<uint32_t>();
	global_water_height = buffer.read<float>();
	water_shader_size = buffer.read<float>();
	water_shader_name = buffer.read<std::string>(false, true);
	seconds_per_world_cycle = buffer.read<float>();

	collidable_min_distance = buffer.read<float>();
	collidable_max_distance = buffer.read<float>();
	collidable_tile_size = buffer.read<float>();
	collidable_tile_border = buffer.read<float>();
	collidable_seed = buffer.read<float>();

	non_collidable_min_distance = buffer.read<float>();
	non_collidable_max_distance = buffer.read<float>();
	non_collidable_tile_size = buffer.read<float>();
	non_collidable_tile_border = buffer.read<float>();
	non_collidable_seed = buffer.read<float>();

	near_radial_min_distance = buffer.read<float>();
	near_radial_max_distance = buffer.read<float>();
	near_radial_tile_size = buffer.read<float>();
	near_radial_tile_border = buffer.read<float>();
	near_radial_seed = buffer.read<float>();

	far_radial_min_distance = buffer.read<float>();
	far_radial_max_distance = buffer.read<float>();
	far_radial_tile_size = buffer.read<float>();
	far_radial_tile_border = buffer.read<float>();
	far_radial_seed = buffer.read<float>();
}

void header::serialize(ByteBuffer& buffer)
{
	buffer.write(reinterpret_cast<const unsigned char*>(filename.c_str()), filename.size());
	buffer.write(uint8_t(0));
	buffer.write(map_width);
	buffer.write(chunk_width);
	buffer.write(tiles_per_chunk);
	buffer.write(use_global_water_height);
	buffer.write(global_water_height);
	buffer.write(water_shader_size);
	buffer.write(reinterpret_cast<const unsigned char*>(water_shader_name.c_str()), water_shader_name.size());
	buffer.write(uint8_t(0));
	buffer.write(seconds_per_world_cycle);

	buffer.write(collidable_min_distance);
	buffer.write(collidable_max_distance);
	buffer.write(collidable_tile_size);
	buffer.write(collidable_tile_border);
	buffer.write(collidable_seed);

	buffer.write(non_collidable_min_distance);
	buffer.write(non_collidable_max_distance);
	buffer.write(non_collidable_tile_size);
	buffer.write(non_collidable_tile_border);
	buffer.write(non_collidable_seed);

	buffer.write(near_radial_min_distance);
	buffer.write(near_radial_max_distance);
	buffer.write(near_radial_tile_size);
	buffer.write(near_radial_tile_border);
	buffer.write(near_radial_seed);

	buffer.write(far_radial_min_distance);
	buffer.write(far_radial_max_distance);
	buffer.write(far_radial_tile_size);
	buffer.write(far_radial_tile_border);
	buffer.write(far_radial_seed);
}


void footer::deserialize(ByteBuffer& buffer)
{
	map_size = buffer.read<float>();
	chunk_width = buffer.read<float>();
	map_width = buffer.read<uint32_t>();
	map_height = buffer.read<uint32_t>();
}

void footer::serialize(ByteBuffer& buffer)
{
	buffer.write(map_size);
	buffer.write(chunk_width);
	buffer.write(map_width);
	buffer.write(map_height);
}


void smap::deserialize(ByteBuffer& buffer)
{
	data = std::vector<uint8_t>(buffer.data(), buffer.data() + buffer.size());
	buffer.read_position(buffer.read_position() + data.size());
}

void smap::serialize(ByteBuffer& buffer)
{
	buffer.write(data.data(), data.size());
}


void wmap::deserialize(ByteBuffer& buffer)
{
	data = std::vector<uint8_t>(buffer.data(), buffer.data() + buffer.size());
	buffer.read_position(buffer.read_position() + data.size());
}

void wmap::serialize(ByteBuffer& buffer)
{
	buffer.write(data.data(), data.size());
}

void shader_family::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	family_name = buffer.read<std::string>(false, true);
	surface_properties_file = buffer.read<std::string>(false, true);
	r = buffer.read<uint8_t>();
	g = buffer.read<uint8_t>();
	b = buffer.read<uint8_t>();
	unknown1 = buffer.read<float>();
	feather_clamp = buffer.read<float>();

	uint32_t child_count = buffer.read<uint32_t>();
	for (uint32_t i = 0; i < child_count; ++i)
	{
		auto child = swganh::make_unique<shader_child>();
		child->name = buffer.read<std::string>(false, true);
		child->weight = buffer.read<float>();

		children.push_back(std::move(child));
	}
}

void shader_family::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(reinterpret_cast<const unsigned char*>(family_name.c_str()), family_name.size());
	buffer.write(uint8_t(0));
	buffer.write(reinterpret_cast<const unsigned char*>(surface_properties_file.c_str()), surface_properties_file.size());
	buffer.write(uint8_t(0));
	buffer.write(r);
	buffer.write(g);
	buffer.write(b);
	buffer.write(unknown1);
	buffer.write(feather_clamp);
	buffer.write(uint32_t(children.size()));

	for (const auto& child : children)
	{
		buffer.write(reinterpret_cast<const unsigned char*>(child->name.c_str()), child->name.size());
		buffer.write(uint8_t(0));
		buffer.write(child->weight);
	}
}

void flora_family::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	family_name = buffer.read<std::string>(false, true);
	r = buffer.read<uint8_t>();
	g = buffer.read<uint8_t>();
	b = buffer.read<uint8_t>();
	density = buffer.read<float>();
	floats_on_water = buffer.read<uint32_t>() == 1 ? true : false;

	uint32_t child_count = buffer.read<uint32_t>();
	for (uint32_t i = 0; i < child_count; ++i)
	{
		auto child = swganh::make_unique<flora_child>();
		child->name = buffer.read<std::string>(false, true);
		child->weight = buffer.read<float>();
		child->align_to_terrain = buffer.read<uint32_t>() == 1 ? true : false;
		child->displacement = buffer.read<float>();
		child->period = buffer.read<float>();
		child->sway_flora = buffer.read<uint32_t>() == 1 ? true : false;
		child->scale_flora = buffer.read<uint32_t>() == 1 ? true : false;
		child->min_scale = buffer.read<float>();
		child->max_scale = buffer.read<float>();

		children.push_back(std::move(child));
	}
}

void flora_family::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(reinterpret_cast<const unsigned char*>(family_name.c_str()), family_name.size());
	buffer.write(uint8_t(0));
	buffer.write(r);
	buffer.write(g);
	buffer.write(b);
	buffer.write(density);
	buffer.write(uint32_t(floats_on_water ? 1 : 0));
	buffer.write(uint32_t(children.size()));

	for (const auto& child : children)
	{
		buffer.write(reinterpret_cast<const unsigned char*>(child->name.c_str()), child->name.size());
		buffer.write(uint8_t(0));
		buffer.write(child->weight);
		buffer.write(uint32_t(child->align_to_terrain ? 1 : 0));
		buffer.write(child->displacement);
		buffer.write(child->period);
		buffer.write(uint32_t(child->sway_flora ? 1 : 0));
		buffer.write(uint32_t(child->scale_flora ? 1 : 0));
		buffer.write(child->min_scale);
		buffer.write(child->max_scale);
	}
}

void radial_family::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	family_name = buffer.read<std::string>(false, true);
	r = buffer.read<uint8_t>();
	g = buffer.read<uint8_t>();
	b = buffer.read<uint8_t>();
	density = buffer.read<float>();

	uint32_t child_count = buffer.read<uint32_t>();
	for (uint32_t i = 0; i < child_count; ++i)
	{
		auto child = swganh::make_unique<radial_child>();
		child->name = buffer.read<std::string>(false, true);
		child->weight = buffer.read<float>();
		child->distance = buffer.read<float>();
		child->width = buffer.read<float>();
		child->height = buffer.read<float>();
		child->maintain_aspect_ration = buffer.read<uint32_t>() == 1 ? true : false;
		child->displacement = buffer.read<float>();
		child->period = buffer.read<float>();
		child->sway_flora = buffer.read<uint32_t>() == 1 ? true : false;
		child->unk9 = buffer.read<uint32_t>();

		children.push_back(std::move(child));
	}
}

void radial_family::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(reinterpret_cast<const unsigned char*>(family_name.c_str()), family_name.size());
	buffer.write(uint8_t(0));
	buffer.write(r);
	buffer.write(g);
	buffer.write(b);
	buffer.write(density);
	buffer.write(uint32_t(children.size()));

	for (const auto& child : children)
	{
		buffer.write(reinterpret_cast<const unsigned char*>(child->name.c_str()), child->name.size());
		buffer.write(uint8_t(0));
		buffer.write(child->weight);
		buffer.write(child->distance);
		buffer.write(child->width);
		buffer.write(child->height);
		buffer.write(uint32_t(child->maintain_aspect_ration ? 1 : 0));
		buffer.write(child->displacement);
		buffer.write(child->period);
		buffer.write(uint32_t(child->sway_flora ? 1 : 0));
		buffer.write(child->unk9);
	}
}

void environment_family::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	family_name = buffer.read<std::string>(false, true);
	r = buffer.read<uint8_t>();
	g = buffer.read<uint8_t>();
	b = buffer.read<uint8_t>();
	feather_clamp = buffer.read<float>();
}

void environment_family::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(reinterpret_cast<const unsigned char*>(family_name.c_str()), family_name.size());
	buffer.write(uint8_t(0));
	buffer.write(r);
	buffer.write(g);
	buffer.write(b);
	buffer.write(feather_clamp);
}

void fractal_family::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	family_name = buffer.read<std::string>(false, true);
}

void fractal_family::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(reinterpret_cast<const unsigned char*>(family_name.c_str()), family_name.size());
	buffer.write(uint8_t(0));
}

void fractal_family::fractal::deserialize(ByteBuffer& buffer)
{
	seed = buffer.read<uint32_t>();
	use_bias = buffer.read<uint32_t>() == 1 ? true : false;
	bias = buffer.read<float>();
	use_gain = buffer.read<uint32_t>() == 1 ? true : false;
	gain = buffer.read<float>();
	unknown1 = buffer.read<uint32_t>();
	octaves = buffer.read<float>();
	amplitude = buffer.read<float>();
	x_scale = buffer.read<float>();
	y_scale = buffer.read<float>();
	x_offset = buffer.read<float>();
	y_offset = buffer.read<float>();
	rule = static_cast<combination_rules>(buffer.read<uint32_t>());
}

void fractal_family::fractal::serialize(ByteBuffer& buffer)
{
	buffer.write(seed);
	buffer.write(uint32_t(use_bias ? 1 : 0));
	buffer.write(bias);
	buffer.write(uint32_t(use_gain ? 1 : 0));
	buffer.write(gain);
	buffer.write(unknown1);
	buffer.write(octaves);
	buffer.write(amplitude);
	buffer.write(x_scale);
	buffer.write(y_scale);
	buffer.write(x_offset);
	buffer.write(y_offset);
	buffer.write(uint32_t(rule));
}

e_layer_type base_terrain_layer::get_layer_type() const
{
	return e_layer_type::root;
}

std::string base_terrain_layer::type_str() const
{
	std::string type;

	switch (get_layer_type())
	{
	case e_layer_type::construction:
		{
			type = "construction";
		}
		break;
	case e_layer_type::affector:
		{
			type = "affector";
		}
		break;
	case e_layer_type::boundary:
		{
			type = "boundary";
		}
		break;
	case e_layer_type::filter:
		{
			type = "filter";
		}
		break;
	case e_layer_type::root:
		{
			type = "root";
		}
	case e_layer_type::COUNT: {} break;
	}

	return type;
}

std::string base_terrain_layer::subtype_str() const
{
	return "root";
}

void base_terrain_layer::deserialize(ByteBuffer& buffer)
{
	enabled = buffer.read<uint32_t>() == 1 ? true : false;
	name = buffer.read<std::string>(false, true);
}

void base_terrain_layer::serialize(ByteBuffer& buffer)
{
	buffer.write<uint32_t>(enabled ? 1 : 0);
	buffer.write(reinterpret_cast<const unsigned char*>(name.c_str()), name.size());
	buffer.write(uint8_t(0));
}

std::string base_affector_layer::subtype_str() const
{
	std::string type;

	switch (get_type())
	{
	case e_affector_type::height_terrace:
		{ type = "height_terrace"; }
		break;
	case e_affector_type::height_constant:
		{ type = "height_constant"; }
		break;
	case e_affector_type::height_fractal:
		{ type = "height_fractal"; }
		break;
	case e_affector_type::color_constant:
		{ type = "color_constant"; }
		break;
	case e_affector_type::color_ramp_height:
		{ type = "color_ramp_height"; }
		break;
	case e_affector_type::color_ramp_fractal:
		{ type = "color_ramp_fractal"; }
		break;
	case e_affector_type::shader_constant:
		{ type = "shader_constant"; }
		break;
	case e_affector_type::shader_replace:
		{ type = "shader_replace"; }
		break;
	case e_affector_type::flora_static_collidable_constant:
		{ type = "flora_static_collidable_constant"; }
		break;
	case e_affector_type::flora_static_non_collidable_constant:
		{ type = "flora_static_non_collidable_constant"; }
		break;
	case e_affector_type::flora_dynamic_near_constant:
		{ type = "flora_dynamic_near_constant"; }
		break;
	case e_affector_type::flora_dynamic_far_constant:
		{ type = "flora_dynamic_far_constant"; }
		break;
	case e_affector_type::exclude:
		{ type = "exclude"; }
		break;
	case e_affector_type::passable:
		{ type = "passable"; }
		break;
	case e_affector_type::road:
		{ type = "road"; }
		break;
	case e_affector_type::river:
		{ type = "river"; }
		break;
	case e_affector_type::environment:
		{ type = "environment"; }
		break;
	case e_affector_type::ribbon:
		{ type = "ribbon"; }
		break;
	case e_affector_type::COUNT: {} break;
	}

	return type;
}

std::string base_boundary_layer::subtype_str() const
{
	std::string type;

	switch (get_type())
	{
	case e_boundary_type::circle:
		{ type = "circle"; }
		break;
	case e_boundary_type::rectangle:
		{ type = "height_constant"; }
		break;
	case e_boundary_type::polygon:
		{ type = "height_fractal"; }
		break;
	case e_boundary_type::polyline:
		{ type = "color_constant"; }
		break;
	case e_boundary_type::COUNT: {} break;
	}

	return type;
}

std::string base_filter_layer::subtype_str() const
{
	std::string type;

	switch (get_type())
	{
	case e_filter_type::height:
		{ type = "height"; }
		break;
	case e_filter_type::fractal:
		{ type = "fractal"; }
		break;
	case e_filter_type::slope:
		{ type = "slope"; }
		break;
	case e_filter_type::direction:
		{ type = "direction"; }
		break;
	case e_filter_type::shader:
		{ type = "shader"; }
		break;
	case e_filter_type::bitmap:
		{ type = "bitmap"; }
		break;
	case e_filter_type::COUNT: {} break;
	}

	return type;
}

void affector_color_constant::deserialize(ByteBuffer& buffer)
{
	operation = static_cast<operations>(buffer.read<uint32_t>());
	r = buffer.read<uint8_t>();
	g = buffer.read<uint8_t>();
	b = buffer.read<uint8_t>();
}

void affector_color_constant::serialize(ByteBuffer& buffer)
{
	buffer.write(uint32_t(operation));
	buffer.write(r);
	buffer.write(g);
	buffer.write(b);
}

void affector_color_ramp_fractal::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	operation = static_cast<operations>(buffer.read<uint32_t>());
	ramp = buffer.read<std::string>(false, true);
}

void affector_color_ramp_fractal::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(uint32_t(operation));
	buffer.write(reinterpret_cast<const unsigned char*>(ramp.c_str()), ramp.size());
	buffer.write(uint8_t(0));
}

void affector_color_ramp_height::deserialize(ByteBuffer& buffer)
{
	operation = static_cast<operations>(buffer.read<uint32_t>());
	low_height = buffer.read<float>();
	high_height = buffer.read<float>();
	ramp = buffer.read<std::string>(false, true);
}

void affector_color_ramp_height::serialize(ByteBuffer& buffer)
{
	buffer.write(uint32_t(operation));
	buffer.write(low_height);
	buffer.write(high_height);
	buffer.write(reinterpret_cast<const unsigned char*>(ramp.c_str()), ramp.size());
	buffer.write(uint8_t(0));
}

void affector_environment::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	feather_clamp_override = buffer.read<uint32_t>() == 1 ? true : false;
	clamp = buffer.read<float>();
}

void affector_environment::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write<uint32_t>(feather_clamp_override ? 1 : 0);
	buffer.write(clamp);
}

void affector_flora_collidable_constant::deserialize(ByteBuffer& buffer)
{
	adjust = buffer.read<uint32_t>();
	operation = static_cast<e_flora_operation>(buffer.read<uint32_t>());
	remove_all_radial_flora = buffer.read<uint32_t>() == 1 ? true : false;
	density_override = buffer.read<uint32_t>() == 1 ? true : false;
	density = buffer.read<float>();
}

void affector_flora_collidable_constant::serialize(ByteBuffer& buffer)
{
	buffer.write(adjust);
	buffer.write(uint32_t(operation));
	buffer.write<uint32_t>(remove_all_radial_flora ? 1 : 0);
	buffer.write<uint32_t>(density_override ? 1 : 0);
	buffer.write(density);
}

void affector_flora_non_collidable_constant::deserialize(ByteBuffer& buffer)
{
	adjust = buffer.read<uint32_t>();
	operation = static_cast<e_flora_operation>(buffer.read<uint32_t>());
	remove_all_radial_flora = buffer.read<uint32_t>() == 1 ? true : false;
	density_override = buffer.read<uint32_t>() == 1 ? true : false;
	density = buffer.read<float>();
}

void affector_flora_non_collidable_constant::serialize(ByteBuffer& buffer)
{
	buffer.write(adjust);
	buffer.write(uint32_t(operation));
	buffer.write<uint32_t>(remove_all_radial_flora ? 1 : 0);
	buffer.write<uint32_t>(density_override ? 1 : 0);
	buffer.write(density);
}

void affector_height_constant::deserialize(ByteBuffer& buffer)
{
	operation = static_cast<operations>(buffer.read<uint32_t>());
	value = buffer.read<float>();
}

void affector_height_constant::serialize(ByteBuffer& buffer)
{
	buffer.write(uint32_t(operation));
	buffer.write(value);
}

void affector_height_fractal::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	operation = static_cast<operations>(buffer.read<uint32_t>());
	y_scale = buffer.read<float>();
}

void affector_height_fractal::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(uint32_t(operation));
	buffer.write(y_scale);
}

void affector_height_terrace::deserialize(ByteBuffer& buffer)
{
	terrace_height = buffer.read<float>();
	flat_ratio = buffer.read<float>();
}

void affector_height_terrace::serialize(ByteBuffer& buffer)
{
	buffer.write(terrace_height);
	buffer.write(flat_ratio);
}

void affector_radial_far_constant::deserialize(ByteBuffer& buffer)
{
	adjust = buffer.read<uint32_t>();
	operation = static_cast<e_flora_operation>(buffer.read<uint32_t>());
	remove_all_radial_flora = buffer.read<uint32_t>() == 1 ? true : false;
	density_override = buffer.read<uint32_t>() == 1 ? true : false;
	density = buffer.read<float>();
}

void affector_radial_far_constant::serialize(ByteBuffer& buffer)
{
	buffer.write(adjust);
	buffer.write(uint32_t(operation));
	buffer.write<uint32_t>(remove_all_radial_flora ? 1 : 0);
	buffer.write<uint32_t>(density_override ? 1 : 0);
	buffer.write(density);
}

void affector_radial_near_constant::deserialize(ByteBuffer& buffer)
{
	adjust = buffer.read<uint32_t>();
	operation = static_cast<e_flora_operation>(buffer.read<uint32_t>());
	remove_all_radial_flora = buffer.read<uint32_t>() == 1 ? true : false;
	density_override = buffer.read<uint32_t>() == 1 ? true : false;
	density = buffer.read<float>();
}

void affector_radial_near_constant::serialize(ByteBuffer& buffer)
{
	buffer.write(adjust);
	buffer.write(uint32_t(operation));
	buffer.write<uint32_t>(remove_all_radial_flora ? 1 : 0);
	buffer.write<uint32_t>(density_override ? 1 : 0);
	buffer.write(density);
}

void affector_river::deserialize(ByteBuffer& buffer)
{
	uint32_t size = buffer.read<uint32_t>();

	for (uint32_t i = 0; i < size; ++i)
	{
		glm::vec2 control_point;
		control_point.x = buffer.read<float>();
		control_point.y = buffer.read<float>();

		control_points.push_back(control_point);
	}

	feathering_distance = buffer.read<float>();
	feathering_function = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	bank_shader = buffer.read<uint32_t>();
	bottom_shader = buffer.read<uint32_t>();
	width = buffer.read<float>();
	trench_depth = buffer.read<float>();
	velocity = buffer.read<float>();
	has_water = buffer.read<uint32_t>() == 1 ? true : false;
	water_depth = buffer.read<float>();
	water_width = buffer.read<float>();
	water_shader_size = buffer.read<float>();
	water_shader = buffer.read<std::string>(false, true);
}

void affector_river::serialize(ByteBuffer& buffer)
{
	buffer.write(uint32_t(control_points.size()));

	for (const auto& point : control_points)
	{
		buffer.write(point.x);
		buffer.write(point.y);
	}

	buffer.write(feathering_distance);
	buffer.write(uint32_t(feathering_function));
	buffer.write(bank_shader);
	buffer.write(bottom_shader);
	buffer.write(width);
	buffer.write(trench_depth);
	buffer.write(velocity);
	buffer.write<uint32_t>(has_water ? 1 : 0);
	buffer.write(water_depth);
	buffer.write(water_width);
	buffer.write(water_shader_size);
	buffer.write(reinterpret_cast<const unsigned char*>(water_shader.c_str()), water_shader.size());
	buffer.write(uint8_t(0));
}

void affector_river::segment::deserialize(ByteBuffer& buffer)
{
	while (buffer.read_position() < buffer.size())
	{
		control_point point;
		point.x_pos = buffer.read<float>();
		point.z_pos = buffer.read<float>();
		point.height = buffer.read<float>();

		control_points.push_back(point);
	}
}

void affector_river::segment::serialize(ByteBuffer& buffer)
{
	for (const auto& point : control_points)
	{
		buffer.write(point.x_pos);
		buffer.write(point.z_pos);
		buffer.write(point.height);
	}
}

void affector_road::deserialize(ByteBuffer& buffer)
{
	uint32_t size = buffer.read<uint32_t>();

	for (uint32_t i = 0; i < size; ++i)
	{
		glm::vec2 control_point;
		control_point.x = buffer.read<float>();
		control_point.y = buffer.read<float>();

		control_points.push_back(control_point);
	}

	width = buffer.read<float>();
	shader = buffer.read<uint32_t>();

	height_feathering_function = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	height_feathering_distance = buffer.read<float>();

	shader_feathering_function = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	shader_feathering_distance = buffer.read<float>();
}

void affector_road::serialize(ByteBuffer& buffer)
{
	buffer.write(uint32_t(control_points.size()));

	for (const auto& point : control_points)
	{
		buffer.write(point.x);
		buffer.write(point.y);
	}

	buffer.write(width);
	buffer.write(shader);

	buffer.write(uint32_t(height_feathering_function));
	buffer.write(height_feathering_distance);

	buffer.write(uint32_t(shader_feathering_function));
	buffer.write(shader_feathering_distance);
}

void affector_road::segment::deserialize(ByteBuffer& buffer)
{
	while (buffer.read_position() < buffer.size())
	{
		control_point point;
		point.x_pos = buffer.read<float>();
		point.z_pos = buffer.read<float>();
		point.height = buffer.read<float>();

		control_points.push_back(point);
	}
}

void affector_road::segment::serialize(ByteBuffer& buffer)
{
	for (const auto& point : control_points)
	{
		buffer.write(point.x_pos);
		buffer.write(point.z_pos);
		buffer.write(point.height);
	}
}

void affector_shader_constant::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	feather_clamp_override = buffer.read<uint32_t>() == 1 ? true : false;
	clamp = buffer.read<float>();
}

void affector_shader_constant::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write<uint32_t>(feather_clamp_override ? 1 : 0);
	buffer.write(clamp);
}

void affector_shader_replace::deserialize(ByteBuffer& buffer)
{
	source_family = buffer.read<uint32_t>();
	destination_family = buffer.read<uint32_t>();
	feather_clamp_override = buffer.read<uint32_t>() == 1 ? true : false;
	clamp = buffer.read<float>();
}

void affector_shader_replace::serialize(ByteBuffer& buffer)
{
	buffer.write(source_family);
	buffer.write(destination_family);
	buffer.write<uint32_t>(feather_clamp_override ? 1 : 0);
	buffer.write(clamp);
}

void boundary_circle::deserialize(ByteBuffer& buffer)
{
	center_x = buffer.read<float>();
	center_z = buffer.read<float>();
	radius = buffer.read<float>();

	function = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	distance = buffer.read<float>();
}

void boundary_circle::serialize(ByteBuffer& buffer)
{
	buffer.write(center_x);
	buffer.write(center_z);
	buffer.write(radius);

	buffer.write(uint32_t(function));
	buffer.write(distance);
}

void boundary_polygon::deserialize(ByteBuffer& buffer)
{
	uint32_t size = buffer.read<uint32_t>();
	for (uint32_t i = 0; i < size; ++i)
	{
		glm::vec2 control_point;
		control_point.x = buffer.read<float>();
		control_point.y = buffer.read<float>();

		control_points.push_back(control_point);
	}

	function = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	distance = buffer.read<float>();
	is_local_water_table = buffer.read<uint32_t>() == 1 ? true : false;
	water_height = buffer.read<float>();
	water_shader_size = buffer.read<float>();
	water_shader = buffer.read<std::string>(false, true);
}

void boundary_polygon::serialize(ByteBuffer& buffer)
{
	buffer.write<uint32_t>(control_points.size());
	for (const auto& point : control_points)
	{
		buffer.write(point.x);
		buffer.write(point.y);
	}

	buffer.write(uint32_t(function));
	buffer.write(distance);
	buffer.write<uint32_t>(is_local_water_table ? 1 : 0);
	buffer.write(water_height);
	buffer.write(water_shader_size);
	buffer.write(reinterpret_cast<const unsigned char*>(water_shader.c_str()), water_shader.size());
	buffer.write(uint8_t(0));
}

void boundary_polyline::deserialize(ByteBuffer& buffer)
{
	uint32_t size = buffer.read<uint32_t>();
	for (uint32_t i = 0; i < size; ++i)
	{
		glm::vec2 control_point;
		control_point.x = buffer.read<float>();
		control_point.y = buffer.read<float>();

		control_points.push_back(control_point);
	}

	function = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	distance = buffer.read<float>();
	width = buffer.read<float>();
}

void boundary_polyline::serialize(ByteBuffer& buffer)
{
	buffer.write<uint32_t>(control_points.size());
	for (const auto& point : control_points)
	{
		buffer.write(point.x);
		buffer.write(point.y);
	}

	buffer.write(uint32_t(function));
	buffer.write(distance);
	buffer.write(width);
}

void boundary_rectangle::deserialize(ByteBuffer& buffer)
{
	x1 = buffer.read<float>();
	z1 = buffer.read<float>();
	x2 = buffer.read<float>();
	z2 = buffer.read<float>();
	function = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	distance = buffer.read<float>();
	is_local_water_table = buffer.read<uint32_t>() == 1 ? true : false;
	local_global_water_table = buffer.read<uint32_t>() == 1 ? true : false;
	water_height = buffer.read<float>();
	water_shader_size = buffer.read<float>();
	water_shader = buffer.read<std::string>(false, true);
}

void boundary_rectangle::serialize(ByteBuffer& buffer)
{
	buffer.write(x1);
	buffer.write(z1);
	buffer.write(x2);
	buffer.write(z2);
	buffer.write(uint32_t(function));
	buffer.write(distance);
	buffer.write<uint32_t>(is_local_water_table ? 1 : 0);
	buffer.write<uint32_t>(local_global_water_table ? 1 : 0);
	buffer.write(water_height);
	buffer.write(water_shader_size);
	buffer.write(reinterpret_cast<const unsigned char*>(water_shader.c_str()), water_shader.size());
	buffer.write(uint8_t(0));
}

void construction_layer::deserialize(ByteBuffer& buffer)
{
	invert_boundaries = buffer.read<uint32_t>() == 1 ? true : false;
	invert_filters = buffer.read<uint32_t>() == 1 ? true : false;
	unknown1 = buffer.read<uint32_t>();
	notes = buffer.read<std::string>(false, true);
}

void construction_layer::serialize(ByteBuffer& buffer)
{
	buffer.write<uint32_t>(invert_boundaries ? 1 : 0);
	buffer.write<uint32_t>(invert_filters ? 1 : 0);
	buffer.write(unknown1);
	buffer.write(reinterpret_cast<const unsigned char*>(notes.c_str()), notes.size());
	buffer.write(uint8_t(0));
}

void filter_direction::deserialize(ByteBuffer& buffer)
{
	min_angle = buffer.read<float>();
	max_angle = buffer.read<float>();
	feathering = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	feather_distance = buffer.read<float>();
}

void filter_direction::serialize(ByteBuffer& buffer)
{
	buffer.write(min_angle);
	buffer.write(max_angle);
	buffer.write(uint32_t(feathering));
	buffer.write(feather_distance);
}

void filter_fractal::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
	feathering = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	feather_distance = buffer.read<float>();
	low = buffer.read<float>();
	high = buffer.read<float>();
	step = buffer.read<float>();
}

void filter_fractal::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
	buffer.write(uint32_t(feathering));
	buffer.write(feather_distance);
	buffer.write(low);
	buffer.write(high);
	buffer.write(step);
}

void filter_height::deserialize(ByteBuffer& buffer)
{
	low_height = buffer.read<float>();
	high_height = buffer.read<float>();
	feathering = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	feather_distance = buffer.read<float>();
}

void filter_height::serialize(ByteBuffer& buffer)
{
	buffer.write(low_height);
	buffer.write(high_height);
	buffer.write(uint32_t(feathering));
	buffer.write(feather_distance);
}

void filter_shader::deserialize(ByteBuffer& buffer)
{
	family_id = buffer.read<uint32_t>();
}

void filter_shader::serialize(ByteBuffer& buffer)
{
	buffer.write(family_id);
}

void filter_slope::deserialize(ByteBuffer& buffer)
{
	min_angle = buffer.read<float>();
	max_angle = buffer.read<float>();
	feathering = static_cast<e_feathering_function>(buffer.read<uint32_t>());
	feather_distance = buffer.read<float>();
}

void filter_slope::serialize(ByteBuffer& buffer)
{
	buffer.write(min_angle);
	buffer.write(max_angle);
	buffer.write(uint32_t(feathering));
	buffer.write(feather_distance);
}