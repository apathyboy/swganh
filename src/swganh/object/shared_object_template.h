
#ifndef SWGANH_OBJECT_SHARED_OBJECT_TEMPLATE_H_
#define SWGANH_OBJECT_SHARED_OBJECT_TEMPLATE_H_

#include <cstdint>
#include <string>
#include <boost/optional.hpp>

#include "object_template.h"
#include "anh/property/property_types.h"

namespace swganh {
namespace object {

    class SharedObjectTemplate : public ObjectTemplate
    {
    public:
        explicit SharedObjectTemplate(std::string name);
        ~SharedObjectTemplate();

    protected:
        void BuildPropertyMap();
        
    private:
        boost::optional<anh::property::StringList> object_name_;
        boost::optional<anh::property::StringList> detailed_description_;
        boost::optional<anh::property::StringList> look_at_text_;
        boost::optional<bool> snap_to_terrain_;
        boost::optional<uint32_t> container_type_;
        boost::optional<uint32_t> container_volume_limit_;
        boost::optional<std::string> tint_palette_;
        boost::optional<std::string> slot_descriptor_filename_;
        boost::optional<std::string> arrangement_descriptor_filename_;
        boost::optional<std::string> appearance_filename_;
        boost::optional<std::string> portal_layout_filename_;
        boost::optional<std::string> client_data_file_;
        boost::optional<uint32_t> collision_material_flags_;
        boost::optional<uint32_t> collision_material_pass_flags_;
        boost::optional<uint32_t> collision_material_block_flags_;
        boost::optional<uint32_t> collision_action_flags_;
        boost::optional<uint32_t> collision_action_pass_flags_;
        boost::optional<uint32_t> collision_action_block_flags_;
        boost::optional<float> scale_;
        boost::optional<uint32_t> game_object_type_;
        boost::optional<bool> send_to_client_;
        boost::optional<float> scale_threshold_before_extent_test_;
        boost::optional<float> clear_flora_radius_;
        boost::optional<uint32_t> surface_type_;
        boost::optional<float> no_build_radius_;
        boost::optional<bool> only_visible_in_tools_;
        boost::optional<float> location_reservation_radius_;
    };

}}

#endif  // SWGANH_OBJECT_SHARED_OBJECT_TEMPLATE_H_
