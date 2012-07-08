// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_OBJECT_OBJECT_TEMPLATE_H_
#define SWGANH_OBJECT_OBJECT_TEMPLATE_H_

#include <string>
#include <boost/optional.hpp>
#include "anh/property/property_container.h"

namespace swganh {
namespace object {
    
    class ObjectTemplate : public anh::property::PropertyContainer
	{
    public:
        explicit ObjectTemplate(std::string name);
        virtual ~ObjectTemplate();
    
        std::string GetName() const;
    
        ObjectTemplate* GetDerived() const;
        void SetDerived(ObjectTemplate* derived);

    private:
        virtual void BuildPropertyMap() = 0;
    
        anh::property::Property* DoGetProperty(const std::string& name);
    
        std::string name_;
        ObjectTemplate* derived_;
    };

    
    namespace template_utils {
        template<typename T>
        T GetNestedPropertyValue(boost::optional<T>& property, const std::string& name, ObjectTemplate* derived)
        {
            if (property.is_initialized())
                return *property;
            else if (!derived)
                throw std::runtime_error("Invalid property request: " + name);
        
            return derived->GetPropertyAs<T>(name);
        }
        
        template<typename T>
        void SetPropertyValue(boost::optional<T>& property, T value)
        {
            property = value;
        }
    }
}}

#endif  // SWGANH_OBJECT_OBJECT_TEMPLATE_H_
