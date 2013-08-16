// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "swganh/byte_buffer.h"

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

        iff_node* form(const char name[4]);
        iff_node* record(const char name[4]);

        std::string str_name() { return std::string(reinterpret_cast<const char*>(&name), sizeof(name)); }
        std::string str_form_type() { return std::string(reinterpret_cast<const char*>(&form_type), sizeof(form_type)); }
    };

    std::unique_ptr<iff_node> make_form(const char form_type[4], iff_node* parent = nullptr);
    std::unique_ptr<iff_node> make_version_form(const char form_type[4], const char form_version[4], iff_node* parent = nullptr);
    std::unique_ptr<iff_node> make_record(const char name[4], iff_node* parent = nullptr);

    std::unique_ptr<iff_node> parse_iff(ByteBuffer& resource);
    void write_iff(ByteBuffer& resource, iff_node* root);

}}  // namespace swganh::tre
