
#include "shared_object_template.h"

using namespace anh::property;
using namespace swganh::object;

SharedObjectTemplate::SharedObjectTemplate(std::string name, ObjectTemplate* derived)
    : ObjectTemplate(name, derived)
{}

SharedObjectTemplate::~SharedObjectTemplate()
{}

void SharedObjectTemplate::BuildPropertyMap()
{
    static std::string GROUPNAME = "SHOT";
    
    AddProperty(
        std::make_shared<StringListProperty>("objectName", "Object Name", "A general name for this object type", GROUPNAME,
        [this] () { return object_name_; },
        [this] (StringList object_name) 
    {
        object_name_ = object_name;
    }));
    
}
