// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "property_container.h"

using namespace anh::property;

PropertyContainer::~PropertyContainer() {}

void PropertyContainer::RemoveProperty(const std::string& name)
{
    boost::unique_lock<boost::shared_mutex> lock(properties_mutex_);
    properties_.erase(name);
}
        
Property* PropertyContainer::DoGetProperty(const std::string& name) const
{
    boost::shared_lock<boost::shared_mutex> lock(properties_mutex_);
    auto find_iter = properties_.find(name);

    if (find_iter == std::end(properties_))
    {
        return nullptr;
    }

    return find_iter->second.get();
}
