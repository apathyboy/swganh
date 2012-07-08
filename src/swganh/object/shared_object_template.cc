
#include "shared_object_template.h"

using namespace anh::property;
using namespace swganh::object;

SharedObjectTemplate::SharedObjectTemplate(std::string name, ObjectTemplate* derived)
    : ObjectTemplate(name, derived)
{}

SharedObjectTemplate::~SharedObjectTemplate()
{}

void SharedObjectTemplate::BuildPropertyMap()
{
    static const std::string GROUPNAME = "SHOT";
    
    AddProperty(
        std::make_shared<StringListProperty>("objectName", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringListProperty::property_type>, std::ref(object_name_), "objectName", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringListProperty::property_type>, std::ref(object_name_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringListProperty>("detailedDescription", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringListProperty::property_type>, std::ref(detailed_description_), "detailedDescription", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringListProperty::property_type>, std::ref(detailed_description_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringListProperty>("lookAtText", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringListProperty::property_type>, std::ref(look_at_text_), "lookAtText", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringListProperty::property_type>, std::ref(look_at_text_), std::placeholders::_1)));
        
    AddProperty(
        std::make_shared<BoolProperty>("snapToTerrain", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<BoolProperty::property_type>, std::ref(snap_to_terrain_), "snapToTerrain", GetDerived()),
        std::bind(template_utils::SetPropertyValue<BoolProperty::property_type>, std::ref(snap_to_terrain_), std::placeholders::_1)));
        
    AddProperty(
        std::make_shared<UInt32Property>("containerType", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(container_type_), "containerType", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(container_type_), std::placeholders::_1)));
       
    AddProperty(
        std::make_shared<UInt32Property>("containerVolumeLimit", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(container_volume_limit_), "containerVolumeLimit", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(container_volume_limit_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringProperty>("tintPalette", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringProperty::property_type>, std::ref(tint_palette_), "tintPalette", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringProperty::property_type>, std::ref(tint_palette_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringProperty>("slotDescriptorFilename", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringProperty::property_type>, std::ref(slot_descriptor_filename_), "slotDescriptorFilename", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringProperty::property_type>, std::ref(slot_descriptor_filename_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringProperty>("arrangementDescriptorFilename", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringProperty::property_type>, std::ref(arrangement_descriptor_filename_), "arrangementDescriptorFilename", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringProperty::property_type>, std::ref(arrangement_descriptor_filename_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringProperty>("appearanceFilename", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringProperty::property_type>, std::ref(appearance_filename_), "appearanceFilename", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringProperty::property_type>, std::ref(appearance_filename_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringProperty>("portalLayoutFilename", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringProperty::property_type>, std::ref(portal_layout_filename_), "portalLayoutFilename", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringProperty::property_type>, std::ref(portal_layout_filename_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<StringProperty>("clientDataFile", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringProperty::property_type>, std::ref(client_data_file_), "clientDataFile", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringProperty::property_type>, std::ref(client_data_file_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("collisionMaterialFlags", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(collision_material_flags_), "collisionMaterialFlags", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(collision_material_flags_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("collisionMaterialPassFlags", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(collision_material_pass_flags_), "collisionMaterialPassFlags", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(collision_material_pass_flags_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("collisionMaterialBlockFlags", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(collision_material_block_flags_), "collisionMaterialBlockFlags", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(collision_material_block_flags_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("collisionActionFlags", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(collision_action_flags_), "collisionActionFlags", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(collision_action_flags_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("collisionActionPassFlags", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(collision_action_pass_flags_), "collisionActionPassFlags", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(collision_action_pass_flags_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("collisionActionBlockFlags", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(collision_action_block_flags_), "scale", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(collision_action_block_flags_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<FloatProperty>("scale", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<FloatProperty::property_type>, std::ref(scale_), "collisionActionBlockFlags", GetDerived()),
        std::bind(template_utils::SetPropertyValue<FloatProperty::property_type>, std::ref(scale_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("gameObjectType", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(game_object_type_), "gameObjectType", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(game_object_type_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<BoolProperty>("sendToClient", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<BoolProperty::property_type>, std::ref(send_to_client_), "sendToClient", GetDerived()),
        std::bind(template_utils::SetPropertyValue<BoolProperty::property_type>, std::ref(send_to_client_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<FloatProperty>("scaleThresholdBeforeExtentTest", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<FloatProperty::property_type>, std::ref(scale_threshold_before_extent_test_), "scaleThresholdBeforeExtentTest", GetDerived()),
        std::bind(template_utils::SetPropertyValue<FloatProperty::property_type>, std::ref(scale_threshold_before_extent_test_), std::placeholders::_1)));

    AddProperty(
        std::make_shared<FloatProperty>("clearFloraRadius", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<FloatProperty::property_type>, std::ref(clear_flora_radius_), "clearFloraRadius", GetDerived()),
        std::bind(template_utils::SetPropertyValue<FloatProperty::property_type>, std::ref(clear_flora_radius_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("surfaceType", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(surface_type_), "surfaceType", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(surface_type_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<FloatProperty>("noBuildRadius", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<FloatProperty::property_type>, std::ref(no_build_radius_), "noBuildRadius", GetDerived()),
        std::bind(template_utils::SetPropertyValue<FloatProperty::property_type>, std::ref(no_build_radius_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<BoolProperty>("onlyVisibleInTools", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<BoolProperty::property_type>, std::ref(only_visible_in_tools_), "onlyVisibleInTools", GetDerived()),
        std::bind(template_utils::SetPropertyValue<BoolProperty::property_type>, std::ref(only_visible_in_tools_), std::placeholders::_1)));

    AddProperty(
        std::make_shared<FloatProperty>("locationReservationRadius", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<FloatProperty::property_type>, std::ref(location_reservation_radius_), "locationReservationRadius", GetDerived()),
        std::bind(template_utils::SetPropertyValue<FloatProperty::property_type>, std::ref(location_reservation_radius_), std::placeholders::_1)));

}

