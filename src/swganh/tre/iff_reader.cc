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

void IffReader::ReadNodes_(boost::archive::binary_iarchive& archive, IffReader::Node* parent)
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

IffReader::Node* IffReader::FindForm(const std::string& form_name)
{
    if (std::strncmp(head_->name, "FORM", sizeof(head_->name)) == 0 &&
        std::strncmp(head_->type, form_name.c_str(), sizeof(head_->type)) == 0)
    {
        return head_.get();
    }

    return head_->FindForm(form_name);
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

IffReader::Node* IffReader::Node::FindForm(const std::string& form_name)
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

std::list<IffReader::Node*> IffReader::FindAllRecords(const std::string& record_name)
{
    return head_->FindAllRecords(record_name);
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

IffReader::Node* IffReader::Node::FindRecord(const std::string& record_name)
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
