// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <swganh/byte_buffer.h>
#include <swganh/tre/visitors/visitor_interface.h>

namespace swganh {
namespace tre {

	struct iff_node
	{
		uint32_t name;
		uint32_t form;
		ByteBuffer data;
		iff_node* parent;
		std::vector<std::unique_ptr<iff_node>> children;

		iff_node()
			: name(0)
			, form(0)
			, parent(nullptr)
		{}

		std::string str_name() { return std::string(reinterpret_cast<char*>(&name), sizeof(name)); }
		std::string str_form() { return std::string(reinterpret_cast<char*>(&form), sizeof(form)); }
	};

	std::unique_ptr<iff_node> parse_iff(ByteBuffer& resource, iff_node* parent = nullptr);
	void write_iff(ByteBuffer& resource, iff_node* node);

	class iff_handler_registry
	{};

	class iff_file
	{
	public:

		static void loadIFF(swganh::ByteBuffer input, std::shared_ptr<VisitorInterface> visitor=nullptr);

	private:
		static bool isNodeNameCharacter_(char c);
		static unsigned int getNameSize_(char namedata[], unsigned int size);
		static bool isFolderNode_(std::string& name);
		static std::string getIFFName_(swganh::ByteBuffer& input);
	};

}}  // namespace swganh::tre
