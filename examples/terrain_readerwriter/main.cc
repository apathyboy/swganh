// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "swganh/tre/iff/iff.h"

void read_terrain(std::string terrain_filename);
void print_iff_nodes(swganh::tre::iff_node* head, int depth = 0);
void process(swganh::tre::iff_node* head);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <path to terrain file>" << std::endl;
        exit(0);
    }

    auto start_time = std::chrono::high_resolution_clock::now();

	read_terrain(argv[1]);

    auto stop_time = std::chrono::high_resolution_clock::now();

    std::cout << "Duration: " <<
        std::chrono::duration_cast<std::chrono::milliseconds>
            (stop_time - start_time).count() << "ms" << std::endl;

    return 0;
}

void read_terrain(std::string terrain_filename)
{
	std::ifstream in(terrain_filename, std::ios::binary);

	if (!in.is_open())
	{
		std::cout << "Invalid filename given: " << terrain_filename << std::endl;
		return;
	}

	std::vector<char> tmp { std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };

	in.close();

	swganh::ByteBuffer data(reinterpret_cast<unsigned char*>(tmp.data()), tmp.size());

	auto iff_head = swganh::tre::parse_iff(data);

	if (iff_head)
	{
		print_iff_nodes(iff_head.get());

		std::ofstream out(terrain_filename + ".alt", std::ios::binary);
		if (!out.is_open())
		{
			std::cout << "Invalid filename given: " << terrain_filename + ".alt" << std::endl;
			return;
		}

		swganh::ByteBuffer out_buffer;
		swganh::tre::write_iff(out_buffer, iff_head.get());

		if (out_buffer.size() > 0)
		{
			out.write(reinterpret_cast<char*>(out_buffer.data()), out_buffer.size());
		}

		out.close();

		process(iff_head.get());
	}
}

void print_iff_nodes(swganh::tre::iff_node* head, int depth)
{
	for (int i = 0; i < depth; ++i)
	{
		std::cout << " ";
	}

	std::cout << head->str_name();

	if (head->form)
	{
		std::cout << " - " << head->str_form();
	}

	std::cout << " - (" << head->children.size() << ") ";

	if (!head->form)
	{
		std::cout << head->data.size();
	}

	std::cout << "\n";

	for (const auto& child : head->children)
	{
		print_iff_nodes(child.get(), depth + 1);
	}
}

void process(swganh::tre::iff_node* head)
{
	//head->record("PTAT0014DATA")
}
