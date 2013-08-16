// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "swganh/tre/iff/iff.h"
#include "swganh/terrain/procedural_terrain.h"

using swganh::terrain::procedural_terrain;

std::unique_ptr<procedural_terrain> read_terrain(std::string terrain_filename);
void write_to_file(std::string filename, swganh::ByteBuffer data);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path to terrain file>" << std::endl;
        exit(0);
    }

    auto start_time = std::chrono::high_resolution_clock::now();

	auto pt = read_terrain(argv[1]);
	write_to_file(argv[1], write_procedural_terrain(*pt));

    auto stop_time = std::chrono::high_resolution_clock::now();

    std::cout << "Duration: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>
            (stop_time - start_time).count() << "ms" << std::endl;

    return 0;
}

std::unique_ptr<procedural_terrain> read_terrain(std::string terrain_filename)
{
	std::ifstream in(terrain_filename, std::ios::binary);

	if (!in.is_open())
	{
		throw std::runtime_error("Invalid filename given: " + terrain_filename);
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };

	in.close();

	swganh::ByteBuffer data(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());

	return swganh::terrain::read_procedural_terrain(data);
}

void write_to_file(std::string filename, swganh::ByteBuffer data)
{
	std::ofstream out(filename + ".alt", std::ios::binary);
	if (!out.is_open())
	{
		std::cout << "Invalid filename given: " << filename + ".alt" << std::endl;
		return;
	}

	if (data.size() > 0)
	{
		out.write(reinterpret_cast<char*>(data.data()), data.size());
	}

	out.close();
}
