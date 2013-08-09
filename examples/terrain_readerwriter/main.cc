// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "swganh/tre/iff/iff.h"
#include "swganh/tre/terrain/procedural_terrain.h"

swganh::tre::procedural_terrain read_terrain(std::string terrain_filename);
void write_to_file(std::string filename, swganh::tre::iff_node* head);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path to terrain file>" << std::endl;
        exit(0);
    }

    auto start_time = std::chrono::high_resolution_clock::now();

	auto pt = read_terrain(argv[1]);
	write_to_file(argv[1], pt.iff_doc());

    auto stop_time = std::chrono::high_resolution_clock::now();

    std::cout << "Duration: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>
            (stop_time - start_time).count() << "ms" << std::endl;

    return 0;
}

swganh::tre::procedural_terrain read_terrain(std::string terrain_filename)
{
	std::ifstream in(terrain_filename, std::ios::binary);

	if (!in.is_open())
	{
		throw std::runtime_error("Invalid filename given: " + terrain_filename);
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };

	in.close();

	swganh::ByteBuffer data(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());

	return swganh::tre::procedural_terrain(data);
}

void write_to_file(std::string filename, swganh::tre::iff_node* head)
{
	std::ofstream out(filename + ".alt", std::ios::binary);
	if (!out.is_open())
	{
		std::cout << "Invalid filename given: " << filename + ".alt" << std::endl;
		return;
	}

	swganh::ByteBuffer out_buffer;
	swganh::tre::write_iff(out_buffer, head);

	if (out_buffer.size() > 0)
	{
		out.write(reinterpret_cast<char*>(out_buffer.data()), out_buffer.size());
	}

	out.close();
}
