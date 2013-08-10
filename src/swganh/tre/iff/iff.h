// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <swganh/byte_buffer.h>
#include <swganh/tre/visitors/visitor_interface.h>

namespace swganh {
namespace tre {

	struct iff_node
	{
		uint32_t name;
		uint32_t form_type;
		ByteBuffer data;
		iff_node* parent;
		std::vector<std::unique_ptr<iff_node>> children;

		iff_node()
			: name(0)
			, form_type(0)
			, parent(nullptr)
		{}

		iff_node* form(char name[4]);
		iff_node* record(char name[4]);

		std::string str_name() { return std::string(reinterpret_cast<char*>(&name), sizeof(name)); }
		std::string str_form_type() { return std::string(reinterpret_cast<char*>(&form_type), sizeof(form_type)); }
	};

	std::unique_ptr<iff_node> parse_iff(ByteBuffer& resource, iff_node* parent = nullptr);
	void write_iff(ByteBuffer& resource, iff_node* node);

	class base_iff_document
	{
	public:
		explicit base_iff_document(ByteBuffer& resource);
		virtual ~base_iff_document() {}


	
	private:
		std::unique_ptr<iff_node> root_;
	};

	class iff_handler_registry
	{
	public:
		typedef std::function < void (iff_node*) > iff_handler_type;

		void handle(iff_node* node);

		void add(uint32_t node_type, iff_handler_type&& handler);
		void remove(uint32_t node_type);
		bool has_handler(uint32_t node_type);

	private:
		std::unordered_map<uint32_t, iff_handler_type> handler_map_;
	};

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
