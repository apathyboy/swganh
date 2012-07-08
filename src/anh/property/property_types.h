// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef ANH_PROPERTY_TYPES_H_
#define ANH_PROPERTY_TYPES_H_

#include "property.h"

#include <list>

#pragma warning( push )
#pragma warning( disable : 4244 )
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#pragma warning( pop )

#include <boost/lexical_cast.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "anh/utilities.h"

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

    template<typename T>
    class IntegerProperty : public BaseProperty<T>
    {
    public:
        typedef T property_type;

        IntegerProperty(uint32_t type, std::string name, std::string group, Getter&& getter, Setter&& setter)
            : BaseProperty<T>(type, name, group, std::move(getter), std::move(setter))
        {}

    private:
        virtual std::string DoToString() const
        {
            return std::to_string(GetValue());
        }

        virtual void DoFromString(const std::string& string_value)
        {
            SetValue(boost::lexical_cast<T>(string_value));
        }
    
        virtual size_t DoSerialize(boost::archive::binary_oarchive& archive) const
        {
            //archive << uint8_t(1);
            //archive << " ";
            //archive << anh::hostToBig(GetValue());
            return 0;
        }

        virtual void DoDeserialize(boost::archive::binary_iarchive& archive, size_t length)
        {
            uint8_t enabled;
            archive >> enabled;

            if (enabled > 0)
            {
                uint8_t spacer;
                T value;

                archive >> spacer;
                archive >> value;

                SetValue(anh::bigToHost(value));
            }
        }
    };

    class Int8Property : public IntegerProperty<int8_t>
    {
    public:
        Int8Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int16Property : public IntegerProperty<int16_t>
    {
    public:
        Int16Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int32Property : public IntegerProperty<int32_t>
    {
    public:
        Int32Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class Int64Property : public IntegerProperty<int64_t>
    {
    public:
        Int64Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt8Property : public IntegerProperty<uint8_t>
    {
    public:
        UInt8Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt16Property : public IntegerProperty<uint16_t>
    {
    public:
        UInt16Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt32Property : public IntegerProperty<uint32_t>
    {
    public:
        UInt32Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class UInt64Property : public IntegerProperty<uint64_t>
    {
    public:
        UInt64Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class FloatProperty : public BaseProperty<float>
    {
    public:
        typedef float property_type;

        FloatProperty(std::string name, std::string group, Getter&& getter, Setter&& setter);

    private:
        virtual std::string DoToString() const;
        virtual void DoFromString(const std::string& string_value);
    
        virtual size_t DoSerialize(boost::archive::binary_oarchive& archive) const;
        virtual void DoDeserialize(boost::archive::binary_iarchive& archive, size_t length);
    };

    class BoolProperty : public BaseProperty<bool>
    {
    public:
        typedef bool property_type;

        BoolProperty(std::string name, std::string group, Getter&& getter, Setter&& setter);

    private:
        virtual std::string DoToString() const;
        virtual void DoFromString(const std::string& string_value);
    
        virtual size_t DoSerialize(boost::archive::binary_oarchive& archive) const;
        virtual void DoDeserialize(boost::archive::binary_iarchive& archive, size_t length);
    };

    class StringProperty : public BaseProperty<std::string>
    {
    public:
        typedef std::string property_type;

        StringProperty(std::string name, std::string group, Getter&& getter, Setter&& setter);

    private:
        virtual std::string DoToString() const;
        virtual void DoFromString(const std::string& string_value);
    
        virtual size_t DoSerialize(boost::archive::binary_oarchive& archive) const;
        virtual void DoDeserialize(boost::archive::binary_iarchive& archive, size_t length);
    };

    class StringListProperty : public BaseProperty<StringList>
    {
    public:
        typedef StringList property_type;

        StringListProperty(std::string name, std::string group, Getter&& getter, Setter&& setter);
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

        UStringProperty(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class Vector2Property : public BaseProperty<glm::vec2>
    {
    public:
        typedef glm::vec2 property_type;

        Vector2Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class Vector3Property : public BaseProperty<glm::vec3>
    {
    public:
        typedef glm::vec3 property_type;

        Vector3Property(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };

    class QuaternionProperty : public BaseProperty<glm::quat>
    {
    public:
        typedef glm::quat property_type;

        QuaternionProperty(std::string name, std::string group, Getter&& getter, Setter&& setter);
    };
}}

#endif  // ANH_PROPERTY_TYPES_H_s
