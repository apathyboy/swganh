// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef ANH_PROPERTY_H_
#define ANH_PROPERTY_H_

#include <cstdint>
#include <functional>
#include <string>

namespace boost {
namespace archive {

    class binary_oarchive;
    class binary_iarchive;

}}

namespace anh {
namespace property {

    class Property
    {
    public:
        Property(
            uint32_t type,
            std::string name,
            std::string group,
            bool read_only = false);
        virtual ~Property();
    
        uint32_t GetType() const;
    
        const std::string& GetName() const;
        const std::string& GetGroup() const;
    
        void SetName(std::string name);
        void SetGroup(std::string group);
    
        bool IsReadOnly() const;
        void SetReadOnly();
    
        std::string ToString() const;
        void FromString(const std::string& string_value);
    
        size_t Serialize(boost::archive::binary_oarchive& archive) const;
        void Deserialize(boost::archive::binary_iarchive& archive, size_t length);
    
    private:
        virtual std::string DoToString() const = 0;
        virtual void DoFromString(const std::string& string_value) = 0;
    
        virtual size_t DoSerialize(boost::archive::binary_oarchive& archive) const = 0;
        virtual void DoDeserialize(boost::archive::binary_iarchive& archive, size_t length) = 0;
    
        uint32_t type_;
        std::string name_;
        std::string group_;
        bool read_only_;
    };

    template<typename T>
    class BaseProperty : public Property
    {
    public:
        typedef std::function<T ()> Getter;
        typedef std::function<void (T)> Setter;
        
        BaseProperty(
            uint32_t type, 
            std::string name, 
            std::string group,
            Getter&& getter, 
            Setter&& setter)
            : Property(type, name, group)
            , getter_(std::move(getter))
            , setter_(std::move(setter))
        {}

        virtual ~BaseProperty() {}

        T GetValue() const
        {
            return getter_();
        }

        void SetValue(T value)
        {
            setter_(value);
        }

    private:
        Getter getter_;
        Setter setter_;
    };

}}

#endif  // ANH_PROPERTY_H_
