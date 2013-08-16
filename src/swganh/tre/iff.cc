// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "iff.h"

#include <stack>

#include "swganh/utilities.h"

using swganh::ByteBuffer;
using swganh::tre::iff_node;

namespace swganh {
namespace tre {
namespace detail {
    std::unique_ptr<iff_node> parse_iff(ByteBuffer& resource, iff_node* parent = nullptr);
    void write_iff(ByteBuffer& resource, iff_node* node);
}}}


iff_node* iff_node::form(const char name[4])
{
    uint32_t name_int = *reinterpret_cast<const uint32_t*>(name);

    for (const auto& child : children)
    {
        if (child->form_type == name_int)
        {
            return child.get();
        }
    }

    return nullptr;
}

iff_node* iff_node::record(const char name[4])
{
    uint32_t name_int = *reinterpret_cast<const uint32_t*>(name);

    for (const auto& child : children)
    {
        if (child->name == name_int)
        {
            return child.get();
        }
    }

    return nullptr;
}

std::unique_ptr<iff_node> swganh::tre::make_form(const char form_type[4], iff_node* parent)
{
    auto new_form = make_unique<iff_node>();

    new_form->name = *reinterpret_cast<const uint32_t*>("FORM");
    new_form->form_type = *reinterpret_cast<const uint32_t*>(form_type);
    new_form->parent = parent;

    return new_form;
}

std::unique_ptr<iff_node> swganh::tre::make_version_form(const char form_type[4], const char form_version[4], iff_node* parent)
{
    auto type_form = make_form(form_type, parent);
    auto version_form = make_form(form_version, type_form.get());

    type_form->children.push_back(std::move(version_form));

    return type_form;
}

std::unique_ptr<iff_node> swganh::tre::make_record(const char name[4], iff_node* parent)
{
    auto new_record = swganh::make_unique<iff_node>();

    new_record->name = *reinterpret_cast<const uint32_t*>(name);
    new_record->parent = parent;

    return new_record;
}

std::unique_ptr<iff_node> swganh::tre::parse_iff(ByteBuffer& resource)
{
    auto root_node = swganh::make_unique<iff_node>();

    auto resource_size = resource.size();
    while (resource.read_position() < resource_size)
    {
        auto top_level_node = detail::parse_iff(resource, root_node.get());
        root_node->children.push_back(std::move(top_level_node));
    }

    return root_node;
}

void swganh::tre::write_iff(ByteBuffer& resource, iff_node* root)
{
    for (const auto& child : root->children)
    {
        detail::write_iff(resource, child.get());
    }
}

std::unique_ptr<iff_node> swganh::tre::detail::parse_iff(ByteBuffer& resource, iff_node* parent)
{
    auto node = swganh::make_unique<iff_node>();

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

void swganh::tre::detail::write_iff(ByteBuffer& resource, iff_node* node)
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
            detail::write_iff(resource, child.get());
        }
    }
    else
    {
        if (node->data.size() > 0)
        {
            resource.write(node->data.data(), node->data.size());
        }
    }

    resource.writeAt(size_offset, swganh::hostToBig<uint32_t>(resource.write_position() - data_start_offset));
}
