
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
