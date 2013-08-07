
#include "terrain_types.h"

using namespace swganh::tre::detail_terrain;
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
		auto child = std::make_unique<shader_child>();
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
		auto child = std::make_unique<flora_child>();
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
		auto child = std::make_unique<radial_child>();
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