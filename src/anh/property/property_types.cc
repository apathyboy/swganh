// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "property_types.h"

#include <sstream>

#pragma warning( push )
#pragma warning( disable : 4244 )
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#pragma warning( pop )

using namespace anh::property;


Int8Property::Int8Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<int8_t>(P_INT8, name, label, description, group, std::move(getter), std::move(setter))
{}

Int16Property::Int16Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<int16_t>(P_INT16, name, label, description, group, std::move(getter), std::move(setter))
{}

Int32Property::Int32Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<int32_t>(P_INT32, name, label, description, group, std::move(getter), std::move(setter))
{}

Int64Property::Int64Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<int64_t>(P_INT64, name, label, description, group, std::move(getter), std::move(setter))
{}


UInt8Property::UInt8Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<uint8_t>(P_UINT8, name, label, description, group, std::move(getter), std::move(setter))
{}

UInt16Property::UInt16Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<uint16_t>(P_UINT16, name, label, description, group, std::move(getter), std::move(setter))
{}

UInt32Property::UInt32Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<uint32_t>(P_UINT32, name, label, description, group, std::move(getter), std::move(setter))
{}

UInt64Property::UInt64Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<uint64_t>(P_UINT64, name, label, description, group, std::move(getter), std::move(setter))
{}

FloatProperty::FloatProperty(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<float>(P_FLOAT, name, label, description, group, std::move(getter), std::move(setter))
{}

BoolProperty::BoolProperty(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<bool>(P_BOOL, name, label, description, group, std::move(getter), std::move(setter))
{}

StringProperty::StringProperty(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<std::string>(P_BOOL, name, label, description, group, std::move(getter), std::move(setter))
{}

StringListProperty::StringListProperty(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<StringList>(P_BOOL, name, label, description, group, std::move(getter), std::move(setter))
{}

StringListProperty::~StringListProperty()
{}

std::string StringListProperty::DoToString() const
{
    std::stringstream ss;

    auto value = GetValue();

    for (auto& str : value)
    {
        ss << str << " ";
    }

    return ss.str();
}

void StringListProperty::DoFromString(const std::string& string_value)
{

}

size_t StringListProperty::DoSerialize(boost::archive::binary_oarchive& archive) const
{
    return 0;
}

void StringListProperty::DoDeserialize(boost::archive::binary_iarchive& archive, size_t length)
{
    uint8_t enabled;

    archive >> enabled;

    if (enabled > 0)
    {
        StringList value;
        std::string tmp;
        uint8_t spacer;

        archive >> spacer;
        archive >> tmp;

        value.push_back(tmp);

        archive >> spacer;
        archive >> tmp;
        
        value.push_back(tmp);

        SetValue(value);
    }
}

UStringProperty::UStringProperty(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<std::wstring>(P_BOOL, name, label, description, group, std::move(getter), std::move(setter))
{}

Vector2Property::Vector2Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<glm::vec2>(P_BOOL, name, label, description, group, std::move(getter), std::move(setter))
{}

Vector3Property::Vector3Property(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<glm::vec3>(P_BOOL, name, label, description, group, std::move(getter), std::move(setter))
{}

QuaternionProperty::QuaternionProperty(
    std::string name, 
    std::string label, 
    std::string description, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<glm::quat>(P_BOOL, name, label, description, group, std::move(getter), std::move(setter))
{}


