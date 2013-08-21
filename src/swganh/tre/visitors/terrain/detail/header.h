// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <string>
#include <swganh/byte_buffer.h>

namespace swganh
{
namespace tre
{

	struct TrnFooter
	{
		void Deserialize(swganh::ByteBuffer buffer)
		{
			map_size = buffer.read<float>();
			chunk_width = buffer.read<float>();
			map_width = buffer.read<uint32_t>();
			map_height = buffer.read<uint32_t>();
		}

		float map_size;
		float chunk_width;
		uint32_t map_width;
		uint32_t map_height;
	};

struct TrnHeader
{
	void Deserialize(swganh::ByteBuffer buffer)
	{
		filename = buffer.read<std::string>(true);
		map_width = buffer.read<float>();
		chunk_width = buffer.read<float>();
		tiles_per_chunk = buffer.read<uint32_t>();
		use_global_water_height = buffer.read<uint32_t>();
		global_water_height = buffer.read<float>();
		water_shader_size = buffer.read<float>();
		water_shader_name = buffer.read<std::string>(true);
		seconds_per_world_cycle = buffer.read<float>();

		Collidable_MinDist = buffer.read<float>();
		Collidable_MaxDist = buffer.read<float>();
		Collidable_TileSize = buffer.read<float>();
		Collidable_TileBorder = buffer.read<float>();
		Collidable_Seed = buffer.read<float>();

		NonCollidable_MinDist = buffer.read<float>();
		NonCollidable_MaxDist = buffer.read<float>();
		NonCollidable_TileSize = buffer.read<float>();
		NonCollidable_TileBorder = buffer.read<float>();
		NonCollidable_Seed = buffer.read<float>();

		NearRadial_MinDist = buffer.read<float>();
		NearRadial_MaxDist = buffer.read<float>();
		NearRadial_TileSize = buffer.read<float>();
		NearRadial_TileBorder = buffer.read<float>();
		NearRadial_Seed = buffer.read<float>();

		FarRadial_MinDist = buffer.read<float>();
		FarRadial_MaxDist = buffer.read<float>();
		FarRadial_TileSize = buffer.read<float>();
		FarRadial_TileBorder = buffer.read<float>();
		FarRadial_Seed = buffer.read<float>();
	}

	std::string filename;
	float map_width;
	float chunk_width;
	uint32_t   tiles_per_chunk;
	uint32_t   use_global_water_height;
	float global_water_height;
	float water_shader_size;
	std::string water_shader_name;
	float seconds_per_world_cycle;

	float Collidable_MinDist;
	float Collidable_MaxDist;
	float Collidable_TileSize;
	float Collidable_TileBorder;
	float Collidable_Seed;

	float NonCollidable_MinDist;
	float NonCollidable_MaxDist;
	float NonCollidable_TileSize;
	float NonCollidable_TileBorder;
	float NonCollidable_Seed;

	float NearRadial_MinDist;
	float NearRadial_MaxDist;
	float NearRadial_TileSize;
	float NearRadial_TileBorder;
	float NearRadial_Seed;

	float FarRadial_MinDist;
	float FarRadial_MaxDist;
	float FarRadial_TileSize;
	float FarRadial_TileBorder;
	float FarRadial_Seed;
};

}
}
