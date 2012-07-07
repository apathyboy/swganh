// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_OBJECT_OBJECT_TEMPLATE_H_
#define SWGANH_OBJECT_OBJECT_TEMPLATE_H_

#include "anh/property/property_container.h"

namespace swganh {
namespace object {

    class ObjectTemplate : public anh::property::PropertyContainer
	{
    public:
        explicit ObjectTemplate(std::string name, ObjectTemplate* derived = nullptr);
        virtual ~ObjectTemplate();
    
        std::string GetName() const;
    
        ObjectTemplate* GetDerived() const;
    
    private:
        virtual void BuildPropertyMap() = 0;
    
        anh::property::Property* DoGetProperty(const std::string& name);
    
        std::string name_;
        ObjectTemplate* derived_;
    };

}}

#endif  // SWGANH_OBJECT_OBJECT_TEMPLATE_H_
