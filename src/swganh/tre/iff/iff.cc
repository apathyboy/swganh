// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "iff.h"

#include <stack>

#include "swganh/utilities.h"

using namespace swganh::tre;

#ifdef WIN32
using std::make_unique;
#else
using swganh::make_unique;
#endif

std::unique_ptr<iff_node> swganh::tre::make_form(char form_type[4], iff_node* parent)
{
	auto new_form = make_unique<iff_node>();

	new_form->name = *reinterpret_cast<const uint32_t*>("FORM");
	new_form->form_type = *reinterpret_cast<const uint32_t*>(form_type);
	new_form->parent = parent;

	return new_form;
}

std::unique_ptr<iff_node> swganh::tre::make_version_form(char form_type[4], char form_version[4], iff_node* parent)
{
	auto type_form = make_form(form_type, parent);
	auto version_form = make_form(form_version, type_form.get());

	type_form->children.push_back(std::move(version_form));

	return type_form;
}

std::unique_ptr<iff_node> swganh::tre::make_record(char name[4], iff_node* parent)
{
	auto new_record = make_unique<iff_node>();

	new_record->name = *reinterpret_cast<uint32_t*>(name);
	new_record->parent = parent;

	return new_record;
}

iff_node* iff_node::form(char name[4])
{
	uint32_t name_int = *reinterpret_cast<uint32_t*>(name);

	for (const auto& child : children)
	{
		if (child->form_type == name_int)
		{
			return child.get();
		}
	}

	return nullptr;
}

iff_node* iff_node::record(char name[4])
{
	uint32_t name_int = *reinterpret_cast<uint32_t*>(name);

	for (const auto& child : children)
	{
		if (child->name == name_int)
		{
			return child.get();
		}
	}

	return nullptr;
}

std::unique_ptr<swganh::tre::iff_node> swganh::tre::parse_iff(ByteBuffer& resource, iff_node* parent)
{
	auto node = make_unique<iff_node>();

	node->parent = parent;

	node->name = resource.read<uint32_t>();
	uint32_t size = swganh::bigToHost(resource.read<uint32_t>());

	if (node->str_name().compare("FORM") == 0)
	{
		uint32_t final_bit = resource.read_position() + size;
		node->form_type = resource.read<uint32_t>();

		while (resource.read_position() < final_bit)
		{
			auto child = parse_iff(resource, node.get());
			node->children.push_back(std::move(child));
		}
	}
	else
	{
		node->data.write(resource.data() + resource.read_position(), size);
		resource.read_position(resource.read_position() + size);
	}

	return node;
}

void swganh::tre::write_iff(ByteBuffer& resource, swganh::tre::iff_node* node)
{
	resource.write(node->name);

	uint32_t size_offset = resource.write_position();
	resource.write(uint32_t(0));
	uint32_t data_start_offset = resource.write_position();

	if (node->str_name().compare("FORM") == 0)
	{
		resource.write(node->form_type);
		for (const auto& child : node->children)
		{
			write_iff(resource, child.get());
		}
	}
	else
	{
		if (node->data.size() > 0)
		{
			resource.write(node->data.data(), node->data.size());
		}
	}

	resource.writeAt(size_offset, swganh::hostToBig(resource.write_position() - data_start_offset));
}



void iff_handler_registry::handle(iff_node* node)
{
	auto find_iter = handler_map_.find(node->name);

	if (find_iter != handler_map_.end())
	{
		find_iter->second(node);
	}
}

void iff_handler_registry::add(uint32_t node_type, iff_handler_type && handler)
{
	handler_map_.insert(std::make_pair(node_type, std::move(handler)));
}

void iff_handler_registry::remove(uint32_t node_type)
{
	handler_map_.erase(node_type);
}

bool iff_handler_registry::has_handler(uint32_t node_type)
{
	return handler_map_.find(node_type) != handler_map_.end();
}

void iff_file::loadIFF(swganh::ByteBuffer inputstream, std::shared_ptr<VisitorInterface> visitor)
{
	//We use a stack instead of recursion to make things more straightforward to follow.
	std::stack<uint32_t> loader;
	int depth = 1;

	//While the stream is not finished
	while(inputstream.read_position() < inputstream.size())
	{
		//We check to see if our read_position has exited a folder node on the stack
		if(!loader.empty() && inputstream.read_position() == loader.top())
		{
			//It has, so we pop the folder node off the stack
			loader.pop();
			--depth;
		}
		else
		{
			//It hasn't, so we get the name of the next node in the stream
			std::string name = getIFFName_(inputstream);

			//It wasn't an iff, or it had an edge case's name. We continue from here as normal
			if(name.size() == 0 || name == "NULL") { continue; }

			//It was, so we read in the size information
			std::uint32_t size = inputstream.read<std::uint32_t>(true);

			//Then we check to see if what it contains is a node or data based the first eight characters
			if(size >= 4 && isFolderNode_(name))
			{
				//We must be a folder.
				loader.push(inputstream.read_position()+size);
				if(visitor != nullptr)
				{
					visitor->visit_folder(depth++, name, size);
				}
			}
			else
			{
				uint32_t post_data_position_ = inputstream.read_position() + size;
				//If we have an interpreter we wil have it interpret our data
				if(visitor != nullptr)
				{
					visitor->visit_data(depth, name, size, inputstream);
				}
				inputstream.read_position(post_data_position_);
			}
		}
	}
}

std::string iff_file::getIFFName_(swganh::ByteBuffer& input)
{
	//Lets read ahead 8 bytes. We could peek here, but we'll need to adjust our stream position anyway
	//so lets just read.
	std::uint64_t name_buf = input.read<std::uint64_t>();

	//Get the size of the name we just read from the stream
	unsigned int namesize = getNameSize_((char*)&name_buf, 8);

	if(namesize < 4)
	{
		//The name size is 0. Lets backtrack and hope for the best
		namesize = 0;
		input.read_position_delta(-8);
	}
	else if(namesize < 8)
	{
		//The name size is 4. Lets backtrack 4 bytes to keep other things happy.
		namesize = 4;
		input.read_position_delta(-4);
	}
	else
	{
		//The name size is 8.
		namesize = 8;
	}

	//Return the string we found
	return std::string((char*)&name_buf, namesize);
}

bool iff_file::isNodeNameCharacter_(char c)
{
	//Returns true if the character is within the proper range for an iff node name
	//in regex : [A-Z0-9 ]
	if((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ')
		return true;
	else
		return false;
}

unsigned int iff_file::getNameSize_(char namedata[], unsigned int size)
{
	//Lets get the name size based on this namedata array
	unsigned int i = 0;
	while(i < size)
	{
		if(!isNodeNameCharacter_(namedata[i]))
		{
			break;
		}
		++i;
	}

	//Return what we think the name size is
	return i;
}

bool iff_file::isFolderNode_(std::string& name)
{
	if(name.substr(0, 4) == "FORM" || name.substr(4, 8) == "FORM")
	{
		return true;
	}
	return false;
}
