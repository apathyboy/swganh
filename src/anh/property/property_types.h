// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef ANH_PROPERTY_TYPES_H_
#define ANH_PROPERTY_TYPES_H_

#include "property.h"

#include <list>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace anh {
namespace property {

    typedef std::list<std::string> StringList;
    
    enum PropertyType
    {
        P_INT8 = 0,
        P_INT16,
        P_INT32,
        P_INT64,
        P_UINT8,
        P_UINT16,
        P_UINT32,
        P_UINT64,
        P_FLOAT,
        P_BOOL,
        P_STRING,
        P_USTRING,
        P_VECTOR2,
        P_VECTOR3,
        P_QUATERNION,
        P_STRING_LIST
    };

    class Int8Property : public BaseProperty<int8_t>
    {
        Int8Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int16Property : public BaseProperty<int16_t>
    {
        Int16Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int32Property : public BaseProperty<int32_t>
    {
        Int32Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int64Property : public BaseProperty<int64_t>
    {
        Int64Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt8Property : public BaseProperty<uint8_t>
    {
        UInt8Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt16Property : public BaseProperty<uint16_t>
    {
        UInt16Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt32Property : public BaseProperty<uint32_t>
    {
        UInt32Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt64Property : public BaseProperty<uint64_t>
    {
        UInt64Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class FloatProperty : public BaseProperty<float>
    {
        FloatProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class BoolProperty : public BaseProperty<bool>
    {
        BoolProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class StringProperty : public BaseProperty<std::string>
    {
        StringProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class StringListProperty : public BaseProperty<StringList>
    {
        StringListProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UStringProperty : public BaseProperty<std::wstring>
    {
        UStringProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Vector2Property : public BaseProperty<glm::vec2>
    {
        Vector2Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Vector3Property : public BaseProperty<glm::vec3>
    {
        Vector3Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class QuaternionProperty : public BaseProperty<glm::quat>
    {
        QuaternionProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };
}}

#endif  // ANH_PROPERTY_TYPES_H_s
