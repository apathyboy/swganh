// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "iff.h"

#include <cstring>
#include <algorithm>
#include <stack>

#include "swganh/utilities.h"

#pragma warning( push )
#pragma warning( disable : 4244 )
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#pragma warning( pop )

using namespace swganh::tre;

namespace bs = boost::serialization;

using boost::archive::binary_iarchive;
using boost::iostreams::basic_array_source;
using boost::iostreams::stream_buffer;
using swganh::tre::IffReader;


std::unique_ptr<swganh::tre::iff_node> swganh::tre::parse_iff(ByteBuffer& resource, iff_node* parent)
{
	auto node = std::make_unique<iff_node>();

	node->name = resource.read<uint32_t>();
	node->size = swganh::bigToHost(resource.read<uint32_t>());
	node->data = resource.data() + resource.read_position();
	node->parent = parent;

	if (node->str_name().compare("FORM") == 0)
	{
		uint32_t final_bit = resource.read_position() + node->size;
		node->form = resource.read<uint32_t>();

		while (resource.read_position() < final_bit)
		{
			auto child = parse_iff(resource, node.get());
			node->children.push_back(std::move(child));
		}
	}
	else
	{
		resource.read_position(resource.read_position() + node->size);
	}

	return node;
}

void swganh::tre::write_iff(ByteBuffer& resource, swganh::tre::iff_node* node)
{
	resource.write(node->name);

	uint32_t write_size = resource.write_position();
	resource.write(uint32_t(0));

	if (node->str_name().compare("FORM") == 0)
	{
		resource.write(node->form);
		for (const auto& child : node->children)
		{
			write_iff(resource, child.get());
		}
	}
	else
	{
		resource.write(node->data, node->size);
	}

	resource.writeAt(write_size, swganh::hostToBig(uint32_t(resource.write_position() - write_size - sizeof(uint32_t))));
}

IffReader::IffReader(const std::vector<char>& input)
{
	stream_buffer<basic_array_source<char>> buffer(&input[0], input.size());
	binary_iarchive archive(buffer, boost::archive::no_header);

	ReadHead_(archive);
}

void IffReader::ReadHead_(binary_iarchive& archive)
{
	head_.reset(new Node);

	archive >> bs::make_array(head_->name, sizeof(head_->name)) >> head_->size;

	head_->size = swganh::bigToHost(head_->size);

	if (std::strncmp(head_->name, "FORM", sizeof(head_->name)) != 0)
	{
		throw BadFileFormat("Invalid Iff file");
	}

	if (head_->size > 0)
	{
		ReadNodes_(archive, head_.get());
	}
}

void IffReader::ReadNodes_(boost::archive::binary_iarchive& archive, IffReader::Node* parent)
{
	size_t current_used = 0;

	archive >> bs::make_array(parent->type, sizeof(parent->type));
	current_used += sizeof(parent->type);

	while (current_used < parent->size)
	{
		std::unique_ptr<Node> node(new Node);

		archive >> bs::make_array(node->name, sizeof(node->name)) >> node->size;

		node->size = swganh::bigToHost(node->size);
		node->parent = parent;

		if (std::strncmp(node->name, "FORM", sizeof(node->name)) == 0)
		{
			ReadNodes_(archive, node.get());
		}
		else
		{
			node->data.resize(node->size);
			archive >> bs::make_array(&node->data[0], node->size);
		}

		current_used += sizeof(node->name) + sizeof(node->size) + node->size;

		parent->children.push_back(std::move(node));
	}
}

IffReader::Node* IffReader::Head()
{
	return head_.get();
}

IffReader::Node* IffReader::Form(const std::string& form_name)
{
	if (std::strncmp(head_->name, "FORM", sizeof(head_->name)) == 0 &&
		std::strncmp(head_->type, form_name.c_str(), sizeof(head_->type)) == 0)
	{
		return head_.get();
	}

	return head_->Form(form_name);
}

std::list<IffReader::Node*> IffReader::FindAllForms(const std::string& form_name)
{
	std::list<IffReader::Node*> nodes;

	if (std::strncmp(head_->name, "FORM", sizeof(head_->name)) == 0 &&
		std::strncmp(head_->type, form_name.c_str(), sizeof(head_->type)) == 0)
	{
		nodes.push_back(head_.get());
	}

	nodes.splice(std::end(nodes), head_->FindAllForms(form_name));

	return nodes;
}

std::list<IffReader::Node*> IffReader::FindAllRecords(const std::string& record_name)
{
	return head_->FindAllRecords(record_name);
}

IffReader::Node* IffReader::Node::Form(const std::string& form_name)
{
	Node* node = nullptr;

	for (auto& child : children)
	{
		if (std::strncmp(child->name, "FORM", sizeof(child->name)) != 0)
		{
			continue;
		}

		if (std::strncmp(child->type, form_name.c_str(), sizeof(child->type)) == 0)
		{
			node = child.get();
		}
		else
		{
			node = child->Form(form_name);
		}

		if (node)
		{
			break;
		}
	}

	if (!node)
	{
		throw InvalidFormType(form_name);
	}

	return node;
}

std::list<IffReader::Node*> IffReader::Node::FindAllForms(const std::string& form_name)
{
	std::list<Node*> nodes;

	for (auto& child : children)
	{
		if (std::strncmp(child->name, "FORM", sizeof(child->name)) == 0 &&
			std::strncmp(child->type, form_name.c_str(), sizeof(child->type)) == 0)
		{
			nodes.push_back(child.get());
		}

		nodes.splice(std::end(nodes), child->FindAllForms(form_name));
	}

	return nodes;
}

IffReader::Node* IffReader::Node::Record(const std::string& record_name)
{
	Node* node = nullptr;

	auto find_iter = std::find_if(std::begin(children), std::end(children),
		[&record_name](const std::unique_ptr<Node>& node)
	{
		return std::strncmp(node->name, record_name.c_str(), sizeof(node->type)) == 0;
	});

	if (find_iter != std::end(children))
	{
		node = (*find_iter).get();
	}

	if (!node)
	{
		throw InvalidRecordType(record_name);
	}

	return node;
}

std::list<IffReader::Node*> IffReader::Node::FindAllRecords(const std::string& record_name)
{
	std::list<Node*> nodes;

	for (auto& child : children)
	{
		if (std::strncmp(child->name, record_name.c_str(), sizeof(child->name)) == 0)
		{
			nodes.push_back(child.get());
		}

		nodes.splice(std::end(nodes), child->FindAllRecords(record_name));
	}

	return nodes;
}
























void iff_file::loadIFF(swganh::ByteBuffer& inputstream, std::shared_ptr<VisitorInterface> visitor)
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
