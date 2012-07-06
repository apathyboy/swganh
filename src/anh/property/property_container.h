// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef ANH_PROPERTY_CONTAINER_H_
#define ANH_PROPERTY_CONTAINER_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <boost/thread/shared_mutex.hpp>

#include "property.h"

namespace anh {
namespace property {

    class PropertyContainer
    {
    public:
        template<typename T>
        void AddProperty(const std::shared_ptr<BaseProperty<T>>& property)
        {
            boost::unique_lock<boost::shared_mutex> lock(properties_mutex_);

            auto find_iter = properties_.find(property->GetName());

            if (find_iter != std::end(properties_))
            {
                properties_.insert(std::make_pair(property->GetName(), property));
            }
            else
            {
                find_iter->second = property;
            }
        }

        template<typename T>
        T GetPropertyAs(const std::string& name)
        {
            auto property = DoGetProperty(name);

            if (!property)
            {
                throw std::runtime_error("Invalid property requested");
            }

            return static_cast<BaseProperty<T>*>(property)->GetValue();
        }

        void RemoveProperty(const std::string& name);

    protected:
        // PropertyContainer is a mixin class and should not deleted polymorphically.
        ~PropertyContainer();
    
    private:
        virtual Property* DoGetProperty(const std::string& name) const;

        typedef std::unordered_map<
            std::string, std::shared_ptr<Property>
        > PropertyMap;

        mutable boost::shared_mutex properties_mutex_;
        PropertyMap properties_;
    };

}}

#endif  // ANH_PROPERTY_CONTAINER_H_
