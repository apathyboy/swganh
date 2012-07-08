
#ifndef SWGANH_OBJECT_SHARED_TANGIBLE_TEMPLATE_H_
#define SWGANH_OBJECT_SHARED_TANGIBLE_TEMPLATE_H_

#include "shared_object_template.h"

namespace swganh {
namespace object {

    class SharedTangibleTemplate : public SharedObjectTemplate
    {
    public:
        explicit SharedTangibleTemplate(std::string name, ObjectTemplate* derived = nullptr);
        ~SharedTangibleTemplate();

    protected:
        void BuildPropertyMap();

    private:
        boost::optional<uint32_t> palette_color_customization_variables_;
        boost::optional<uint32_t> ranged_int_customization_variables_;
        boost::optional<uint32_t> const_string_customization_variables_;
        boost::optional<uint32_t> socket_destinations_;
        boost::optional<std::string> structure_footprint_file_name_;
        boost::optional<bool> use_structure_footprint_outline_;
        boost::optional<bool> targetable_;
        boost::optional<uint32_t> certifications_required_;
        boost::optional<uint32_t> customization_variable_mapping_;
    };
}}

#endif  // SWGANH_OBJECT_SHARED_TANGIBLE_TEMPLATE_H_
