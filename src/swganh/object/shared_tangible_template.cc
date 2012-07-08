
#include "shared_tangible_template.h"

using namespace anh::property;
using namespace swganh::object;

SharedTangibleTemplate::SharedTangibleTemplate(std::string name)
    : SharedObjectTemplate(name)
{}

SharedTangibleTemplate::~SharedTangibleTemplate()
{}

void SharedTangibleTemplate::BuildPropertyMap()
{
    SharedObjectTemplate::BuildPropertyMap();

    static const std::string GROUPNAME = "STOT";

    AddProperty(
        std::make_shared<UInt32Property>("paletteColorCustomizationVariables", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(palette_color_customization_variables_), "paletteColorCustomizationVariables", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(palette_color_customization_variables_), std::placeholders::_1)));
       
    AddProperty(
        std::make_shared<UInt32Property>("rangedIntCustomizationVariables", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(ranged_int_customization_variables_), "rangedIntCustomizationVariables", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(ranged_int_customization_variables_), std::placeholders::_1)));
       
    AddProperty(
        std::make_shared<UInt32Property>("constStringCustomizationVariables", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(const_string_customization_variables_), "constStringCustomizationVariables", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(const_string_customization_variables_), std::placeholders::_1)));
       
    AddProperty(
        std::make_shared<UInt32Property>("socketDestinations", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(socket_destinations_), "socketDestinations", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(socket_destinations_), std::placeholders::_1)));
       
    AddProperty(
        std::make_shared<StringProperty>("structureFootprintFileName", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<StringProperty::property_type>, std::ref(structure_footprint_file_name_), "structureFootprintFileName", GetDerived()),
        std::bind(template_utils::SetPropertyValue<StringProperty::property_type>, std::ref(structure_footprint_file_name_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<BoolProperty>("useStructureFootprintOutline", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<BoolProperty::property_type>, std::ref(use_structure_footprint_outline_), "useStructureFootprintOutline", GetDerived()),
        std::bind(template_utils::SetPropertyValue<BoolProperty::property_type>, std::ref(use_structure_footprint_outline_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<BoolProperty>("targetable", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<BoolProperty::property_type>, std::ref(targetable_), "targetable", GetDerived()),
        std::bind(template_utils::SetPropertyValue<BoolProperty::property_type>, std::ref(targetable_), std::placeholders::_1)));
    
    AddProperty(
        std::make_shared<UInt32Property>("certificationsRequired", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(certifications_required_), "certificationsRequired", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(certifications_required_), std::placeholders::_1)));
       
    AddProperty(
        std::make_shared<UInt32Property>("customizationVariableMapping", GROUPNAME,
        std::bind(template_utils::GetNestedPropertyValue<UInt32Property::property_type>, std::ref(customization_variable_mapping_), "customizationVariableMapping", GetDerived()),
        std::bind(template_utils::SetPropertyValue<UInt32Property::property_type>, std::ref(customization_variable_mapping_), std::placeholders::_1)));
}
