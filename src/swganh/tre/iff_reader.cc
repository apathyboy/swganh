// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "iff_reader.h"

#include <cstring>
#include <algorithm>

#include "anh/utilities.h"

using swganh::tre::IffReader;

IffReader::IffNode::IffNode()
{}

IffReader::IffNode::~IffNode()
{}
    
size_t IffReader::IffNode::GetNodeSize() const
{
    return 0;
}

IffReader::IffNode* IffReader::IffNode::FindNode(const std::string& node_name)
{
    IffNode* node = nullptr;

    for (auto& child : children)
    {
        if (child->name.compare(node_name) == 0)
        {
            return child.get();
        }

        node = child->FindNode(node_name);

        if (node != nullptr)
        {
            return node;
        }
    }

    return node;
}

std::list<IffReader::IffNode*> IffReader::IffNode::FindAllNodes(const std::string& node_name)
{
    std::list<IffReader::IffNode*> nodes;

    for (auto& child : children)
    {
        if (child->name.compare(node_name) == 0)
        {
            nodes.push_back(child.get());
        }

        nodes.splice(std::end(nodes), child->FindAllNodes(node_name));
    }

    return nodes;
}

IffReader::IffReader(std::istream& input)
{
    IffHeader header;
    input.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (std::strcmp(header.name, "FORM") != 0)
    {
        throw BadFileFormat("Invalid Iff file");
    }

    std::unique_ptr<IffNode> node(new IffNode);
    
    node->name = header.name;
    node->size = anh::bigToHost(header.size);
    
    if (node->size > 0)
    {
        ReadNodes_(input, node.get());
    }
    
    heads_.push_back(std::move(node));
}

IffReader::~IffReader()
{}

IffReader::IffNode* IffReader::FindNode(const std::string& node_name)
{
    IffNode* node = nullptr;

    for (auto& head : heads_)
    {
        node = head->FindNode(node_name);

        if (node != nullptr)
        {
            return node;
        }
    }

    return node;
}

std::list<IffReader::IffNode*> IffReader::FindAllNodes(const std::string& node_name)
{
    std::list<IffReader::IffNode*> nodes;

    for (auto& head : heads_)
    {
        nodes.splice(std::end(nodes), head->FindAllNodes(node_name));
    }

    return nodes;
}

size_t IffReader::ReadNodes_(std::istream& input, IffNode* parent)
{
    size_t current_used = 0;

    while(current_used < parent->size)
    {
        size_t remaining = parent->size - current_used;

        char name_buffer[8];
        size_t name_size = 0;

        if (remaining >= 8)
        {
            input.read(name_buffer, sizeof(name_buffer));
            name_size = GetNodeNameSize_(name_buffer);
        }
        else
        {
            parent->data.resize(remaining);
            input.read(&parent->data[0], remaining);

            current_used += remaining;
            continue;
        }

        if (name_size == 0)
        {
            parent->data.resize(remaining);
            std::copy(name_buffer, name_buffer + sizeof(name_buffer), std::begin(parent->data));

            if (remaining > sizeof(name_buffer))
            {
                input.read(&parent->data[sizeof(name_buffer)], remaining - sizeof(name_buffer));
            }

            current_used += remaining;
        }
        else
        {
            std::unique_ptr<IffNode> node(new IffNode);
            node->parent = parent;
            
            std::copy_n(name_buffer, name_size, std::back_inserter(node->name));

            uint32_t data_size = 0;

            if (name_size == 4)
            {
                data_size = *reinterpret_cast<uint32_t*>(&name_buffer[4]);
            }
            else
            {
                input.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));
            }
            
            node->size = anh::bigToHost(data_size);

            ReadNodes_(input, node.get());
            
            current_used += name_size + sizeof(node->size) + node->size;

            parent->children.push_back(std::move(node));
        }
    }

    return current_used;
}

size_t IffReader::GetNodeNameSize_(char* data)
{
	for(unsigned int i=0; i < 8; i++)
	{
		if(	!((data[i] >= 'A' && data[i] <= 'Z') || 
			  (data[i] >= '0' && data[i] <= '9') || data[i] == ' '))
		{
			if( i < 4)
				return 0;
			else
				return 4;
		}
	}

	return 8;
}
