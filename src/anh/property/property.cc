
#include "property.h"

#pragma warning( push )
#pragma warning( disable : 4244 )
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#pragma warning( pop )

using anh::property::Property;


Property::Property(uint32_t type, std::string name, std::string label, std::string description, std::string group, bool read_only)
    : type_(type) 
    , name_(name)
    , label_(label)
    , description_(description)
    , group_(group)
    , read_only_(read_only)
{}

Property::~Property()
{}

uint32_t Property::GetType() const
{
    return type_;
}

const std::string& Property::GetName() const
{
    return name_;
}

const std::string& Property::GetLabel() const
{
    return label_;
}

const std::string& Property::GetDescription() const
{
    return description_;
}

const std::string& Property::GetGroup() const
{
    return group_;
}


void Property::SetName(std::string name)
{
    name_ = name;
}

void Property::SetLabel(std::string label)
{
    label_ = label;
}

void Property::SetDescription(std::string description)
{
    description_ = description;
}

void Property::SetGroup(std::string group)
{
    group_ = group;
}


bool Property::IsReadOnly() const
{
    return read_only_;
}

void Property::SetReadOnly()
{
    read_only_ = true;
}


std::string Property::ToString() const
{
    return DoToString();
}

void Property::FromString(const std::string& string_value)
{
    DoFromString(string_value);
}


size_t Property::Serialize(boost::archive::binary_oarchive& archive) const
{
    return DoSerialize(archive);
}

void Property::Deserialize(boost::archive::binary_iarchive& archive, size_t length)
{
    DoDeserialize(archive, length);
}

