
#include "object_template.h"

using namespace anh::property;
using swganh::object::ObjectTemplate;

ObjectTemplate::ObjectTemplate(std::string name)
    : name_(name)
    , derived_(nullptr)
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

void ObjectTemplate::SetDerived(ObjectTemplate* derived)
{
    derived_ = derived;
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
