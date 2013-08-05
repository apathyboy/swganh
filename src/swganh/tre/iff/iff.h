// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <iostream>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <swganh/byte_buffer.h>
#include <swganh/tre/visitors/visitor_interface.h>


namespace boost {
	namespace archive {
		class binary_iarchive;
	}
}

namespace swganh {
namespace tre {

	struct iff_node
	{
		uint32_t name;
		uint32_t size;
		uint8_t* data;
		uint32_t form;
		iff_node* parent;
		std::vector<std::unique_ptr<iff_node>> children;

		iff_node()
			: name(0)
			, size(0)
			, data(nullptr)
			, form(0)
			, parent(nullptr)
		{}

		std::string str_name() { return std::string(reinterpret_cast<char*>(&name), sizeof(name)); }
		std::string str_form() { return std::string(reinterpret_cast<char*>(&form), sizeof(form)); }
	};

	std::unique_ptr<iff_node> parse_iff(ByteBuffer& resource, iff_node* parent = nullptr);
	void write_iff(ByteBuffer& resource, iff_node* node);

	class iff_file
	{
	public:

		static void loadIFF(swganh::ByteBuffer& input, std::shared_ptr<VisitorInterface> visitor=nullptr);

	private:
		static bool isNodeNameCharacter_(char c);
		static unsigned int getNameSize_(char namedata[], unsigned int size);
		static bool isFolderNode_(std::string& name);
		static std::string getIFFName_(swganh::ByteBuffer& input);
	};

	class IffReader
	{
	public:
		typedef std::runtime_error BadFileFormat;
		typedef std::invalid_argument InvalidFormType;
		typedef std::invalid_argument InvalidRecordType;

		struct Node
		{
			char name[4];
			char type[4];
			uint32_t size;
			Node* parent;
			std::vector<char> data;
			std::vector<std::unique_ptr<Node>> children;

			/**
			* Returns the first form found with the requested name.
			*
			* This method throws an exception when a requested name cannot be found making
			* it ideal for daisy-chaining requests:
			*
			* @code    node->Form("SCOT")->Form("STAT")->Form("SHOT")->Record("DERV");
			*
			* @throws InvalidFormType When no form by the requested name can be found.
			*/
			Node* Form(const std::string& form_name);
			std::list<Node*> FindAllForms(const std::string& form_name);

			Node* Record(const std::string& record_name);
			std::list<Node*> FindAllRecords(const std::string& record_name);
		};

		IffReader(const std::vector<char>& input);

		Node* Head();
		Node* Form(const std::string& form_name);
		std::list<Node*> FindAllForms(const std::string& form_name);
		std::list<Node*> FindAllRecords(const std::string& record_name);

	private:
		IffReader();

		void ReadHead_(boost::archive::binary_iarchive& archive);
		void ReadNodes_(boost::archive::binary_iarchive& archive, Node* parent);

		std::unique_ptr<Node> head_;
	};

}}  // namespace swganh::tre
