// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <fstream>
#include <vector>

#include <glm/glm.hpp>
#include <boost/test/unit_test.hpp>

#include "swganh/byte_buffer.h"
#include "terrain_visitor.h"
#include "swganh/tre/iff/iff.h"

/// By default a new instance of the dispatcher should not have any listeners.
BOOST_AUTO_TEST_CASE(getWaterHeight) {
	std::ifstream in("testdata/terrain/tatooine.trn", std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		BOOST_FAIL("Test terrain file missing");
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
	swganh::ByteBuffer data(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());

	auto terrain_visitor = std::make_shared<swganh::tre::TerrainVisitor>();

	swganh::tre::iff_file::loadIFF(data, terrain_visitor);

	const auto& header = terrain_visitor->GetHeader();

	BOOST_REQUIRE_MESSAGE((0 == header.use_global_water_height), "Expected use_global_water_height of 0, got " << header.use_global_water_height);
	BOOST_REQUIRE_MESSAGE((0 == header.global_water_height), "Expected global_water_height of 0, got " << header.global_water_height);

	in.close();
}

BOOST_AUTO_TEST_CASE(getHeight) {
	std::ifstream in("testdata/terrain/tatooine.trn", std::ios::in | std::ios::binary);

	if (!in.is_open())
	{
		BOOST_FAIL("Test terrain file missing");
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
	swganh::ByteBuffer data(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());

	auto terrain_visitor = std::make_shared<swganh::tre::TerrainVisitor>();

	swganh::tre::iff_file::loadIFF(data, terrain_visitor);

    std::vector<glm::vec3> coords;
    glm::vec3 pos;

    pos.x = 3472.22f; pos.y = 5.f; pos.z = -4918.59f;
    //coords.push_back(pos);
    pos.x = -1278.5f; pos.y = 12.53f; pos.z = -3643.1f;
    coords.push_back(pos);
    pos.x = 3888.2f; pos.y = 33.f; pos.z = 2350.f;
    coords.push_back(pos);
    pos.x = 3951.4f; pos.y = 53.5f; pos.z = 2265.1f;
    coords.push_back(pos);
    pos.x = 3951.11f; pos.y = 54.45f; pos.z = 2282.93f;
    coords.push_back(pos);

    for (unsigned int i = 0; i < coords.size(); i++)
    {
        pos = coords.at(i);
	
		float height_test = terrain_visitor->GetHeight(pos.x, pos.z);
        BOOST_REQUIRE_MESSAGE((fabs(height_test - pos.y) < 1), "Expected height of " << pos.y << " and got " << height_test);
    }

	in.close();
}
