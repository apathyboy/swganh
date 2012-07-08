// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "property_types.h"

#include <sstream>

#include "anh/utilities.h"

using namespace anh::property;


Int8Property::Int8Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<int8_t>(P_INT8, name, group, std::move(getter), std::move(setter))
{}

Int16Property::Int16Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<int16_t>(P_INT16, name, group, std::move(getter), std::move(setter))
{}

Int32Property::Int32Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<int32_t>(P_INT32, name, group, std::move(getter), std::move(setter))
{}

Int64Property::Int64Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<int64_t>(P_INT64, name, group, std::move(getter), std::move(setter))
{}

UInt8Property::UInt8Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<uint8_t>(P_UINT8, name, group, std::move(getter), std::move(setter))
{}

UInt16Property::UInt16Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<uint16_t>(P_UINT16, name, group, std::move(getter), std::move(setter))
{}

UInt32Property::UInt32Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<uint32_t>(P_UINT32, name, group, std::move(getter), std::move(setter))
{}

UInt64Property::UInt64Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : IntegerProperty<uint64_t>(P_UINT64, name, group, std::move(getter), std::move(setter))
{}

FloatProperty::FloatProperty(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<float>(P_FLOAT, name, group, std::move(getter), std::move(setter))
{}

std::string FloatProperty::DoToString() const
{
    return std::to_string(GetValue());
}

void FloatProperty::DoFromString(const std::string& string_value)
{
    SetValue(boost::lexical_cast<float>(string_value));
}

size_t FloatProperty::DoSerialize(boost::archive::binary_oarchive& archive) const
{
    return 0;
}

void FloatProperty::DoDeserialize(boost::archive::binary_iarchive& archive, size_t length)
{
    uint8_t enabled;
    archive >> enabled;

    if (enabled > 0)
    {
        uint8_t spacer;
        float value;

        archive >> spacer;
        archive >> value;

        SetValue(value);
    }
}

BoolProperty::BoolProperty(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<bool>(P_BOOL, name, group, std::move(getter), std::move(setter))
{}

std::string BoolProperty::DoToString() const
{
    return std::to_string(GetValue());
}

void BoolProperty::DoFromString(const std::string& string_value)
{
    SetValue(boost::lexical_cast<bool>(string_value));
}

size_t BoolProperty::DoSerialize(boost::archive::binary_oarchive& archive) const
{
    return 0;
}

void BoolProperty::DoDeserialize(boost::archive::binary_iarchive& archive, size_t length)
{
    uint8_t enabled;
    archive >> enabled;

    if (enabled > 0)
    {
        uint8_t value;

        archive >> value;

        SetValue(value != 0);
    }
}

StringProperty::StringProperty(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<std::string>(P_BOOL, name, group, std::move(getter), std::move(setter))
{}

std::string StringProperty::DoToString() const
{
    return GetValue();
}

void StringProperty::DoFromString(const std::string& string_value)
{
    SetValue(string_value);
}

size_t StringProperty::DoSerialize(boost::archive::binary_oarchive& archive) const
{
    return 0;
}

void StringProperty::DoDeserialize(boost::archive::binary_iarchive& archive, size_t length)
{
    uint8_t enabled;
    archive >> enabled;

    if (enabled > 0)
    {
        std::string value;

        archive >> value;

        SetValue(value);
    }
}

StringListProperty::StringListProperty(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<StringList>(P_BOOL, name, group, std::move(getter), std::move(setter))
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
{}

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
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<std::wstring>(P_BOOL, name, group, std::move(getter), std::move(setter))
{}

Vector2Property::Vector2Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<glm::vec2>(P_BOOL, name, group, std::move(getter), std::move(setter))
{}

Vector3Property::Vector3Property(
    std::string name,
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<glm::vec3>(P_BOOL, name, group, std::move(getter), std::move(setter))
{}

QuaternionProperty::QuaternionProperty(
    std::string name, 
    std::string group, 
    Getter&& getter, 
    Setter&& setter)
    : BaseProperty<glm::quat>(P_BOOL, name, group, std::move(getter), std::move(setter))
{}


