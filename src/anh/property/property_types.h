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
    public:
        typedef int8_t property_type;

        Int8Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int16Property : public BaseProperty<int16_t>
    {
    public:
        typedef int16_t property_type;

        Int16Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int32Property : public BaseProperty<int32_t>
    {
    public:
        typedef int32_t property_type;

        Int32Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int64Property : public BaseProperty<int64_t>
    {
    public:
        typedef int64_t property_type;

        Int64Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt8Property : public BaseProperty<uint8_t>
    {
    public:
        typedef uint8_t property_type;

        UInt8Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt16Property : public BaseProperty<uint16_t>
    {
    public:
        typedef uint16_t property_type;

        UInt16Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt32Property : public BaseProperty<uint32_t>
    {
    public:
        typedef uint32_t property_type;

        UInt32Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt64Property : public BaseProperty<uint64_t>
    {
    public:
        typedef uint64_t property_type;

        UInt64Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class FloatProperty : public BaseProperty<float>
    {
    public:
        typedef float property_type;

        FloatProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class BoolProperty : public BaseProperty<bool>
    {
    public:
        typedef bool property_type;

        BoolProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class StringProperty : public BaseProperty<std::string>
    {
    public:
        typedef std::string property_type;

        StringProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class StringListProperty : public BaseProperty<StringList>
    {
    public:
        typedef StringList property_type;

        StringListProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
        ~StringListProperty();

    private:
        virtual std::string DoToString() const;
        virtual void DoFromString(const std::string& string_value);
    
        virtual size_t DoSerialize(boost::archive::binary_oarchive& archive) const;
        virtual void DoDeserialize(boost::archive::binary_iarchive& archive, size_t length);
    };

    class UStringProperty : public BaseProperty<std::wstring>
    {
    public:
        typedef std::wstring property_type;

        UStringProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Vector2Property : public BaseProperty<glm::vec2>
    {
    public:
        typedef glm::vec2 property_type;

        Vector2Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class Vector3Property : public BaseProperty<glm::vec3>
    {
    public:
        typedef glm::vec3 property_type;

        Vector3Property(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };

    class QuaternionProperty : public BaseProperty<glm::quat>
    {
    public:
        typedef glm::quat property_type;

        QuaternionProperty(std::string name, std::string label, std::string description, std::string group, Getter&& getter, Setter&& setter);
    };
}}

#endif  // ANH_PROPERTY_TYPES_H_s
