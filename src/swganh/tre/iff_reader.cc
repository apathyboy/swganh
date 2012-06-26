// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "iff_reader.h"

#include <cstring>
#include <algorithm>

#include "anh/utilities.h"

#pragma warning( push )
#pragma warning( disable : 4244 )
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#pragma warning( pop )

namespace bs = boost::serialization;

using boost::archive::binary_iarchive;
using boost::iostreams::basic_array_source;
using boost::iostreams::stream_buffer;
using swganh::tre::IffReader;
using swganh::tre::IffReaderV2;

typedef basic_array_source<char> Device;

IffReaderV2::IffReaderV2(const std::vector<char>& input)
{
    stream_buffer<Device> buffer(&input[0], input.size());
    binary_iarchive archive(buffer, boost::archive::no_header);
    
    ReadHead_(archive);
}
    
void IffReaderV2::ReadHead_(binary_iarchive& archive)
{
    head_.reset(new Node);

    archive >> boost::serialization::make_array(head_->name, sizeof(head_->name)) >> head_->size;

    head_->size = anh::bigToHost(head_->size);
        
    if (std::strncmp(head_->name, "FORM", sizeof(head_->name)) != 0)
    {
        throw BadFileFormat("Invalid Iff file");
    }

    if (head_->size > 0)
    {
        ReadNodes_(archive, head_.get());
    }
}

void IffReaderV2::ReadNodes_(boost::archive::binary_iarchive& archive, IffReaderV2::Node* parent)
{
    size_t current_used = 0;
    
    archive >> bs::make_array(parent->type, sizeof(parent->type));
    current_used += sizeof(parent->type);

    while(current_used < parent->size)
    {
        std::unique_ptr<Node> node(new Node);

        archive >> boost::serialization::make_array(node->name, sizeof(node->name)) >> node->size;

        node->size = anh::bigToHost(node->size);
        node->parent = parent;

        if (std::strncmp(node->name, "FORM", sizeof(node->name)) != 0)
        {
            ReadNodes_(archive, node.get());
        }
        else
        {
            archive >> bs::make_array(&node->data[0], node->size);
        }
        
        current_used += sizeof(node->name) + sizeof(node->size) + node->size;
                
        parent->children.push_back(std::move(node));
    }
}

IffReaderV2::Node* IffReaderV2::FindForm(const std::string& form_name)
{
    return head_->FindForm(form_name);
}

std::list<IffReaderV2::Node*> IffReaderV2::FindAllForms(const std::string& form_name)
{
    return head_->FindAllForms(form_name);
}

IffReaderV2::Node* IffReaderV2::Node::FindRecord(const std::string& record_name)
{
    Node* node = nullptr;

    auto find_iter = std::find_if(std::begin(children), std::end(children),
        [&record_name] (const std::unique_ptr<Node>& node)
    {
        return std::strncmp(node->name, record_name.c_str(), sizeof(node->type)) == 0;
    });

    if (find_iter != std::end(children))
    {
        node = (*find_iter).get();
    }

    return node;
}

std::list<IffReaderV2::Node*> IffReaderV2::Node::FindAllRecords(const std::string& record_name)
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

IffReaderV2::Node* IffReaderV2::Node::FindForm(const std::string& form_name)
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
            node = child->FindForm(form_name);
        }

        if (node)
        {
            break;
        }
    }

    return node;
}

std::list<IffReaderV2::Node*> IffReaderV2::Node::FindAllForms(const std::string& form_name)
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
