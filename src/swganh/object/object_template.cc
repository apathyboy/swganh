
#include "object_template.h"

using namespace anh::property;
using swganh::object::ObjectTemplate;

ObjectTemplate::ObjectTemplate(std::string name, ObjectTemplate* derived)
    : name_(name)
    , derived_(derived)
{}

ObjectTemplate::~ObjectTemplate()
{}

std::string ObjectTemplate::GetName() const
{
    return name_;
}

ObjectTemplate* ObjectTemplate::GetDerived() const
{
    return derived_;
}

anh::property::Property* ObjectTemplate::DoGetProperty(const std::string& name)
{
    auto property = PropertyContainer::DoGetProperty(name);

    if (!property)
    {
        property = derived_->DoGetProperty(name);
    }

    return property;
}