// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_TRE_READERS_OBJECT_TEMPLATE_READER_H_
#define SWGANH_TRE_READERS_OBJECT_TEMPLATE_READER_H_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include "swganh/tre/iff_reader.h"

namespace boost {
namespace archive {
    class binary_iarchive;
}}

namespace anh {
namespace resource {
    class ResourceHandle;
    class ResourceManager;
}}

namespace swganh {
namespace object {
    class ObjectTemplate;
}}

namespace swganh {
namespace tre {
namespace readers {

    typedef std::unordered_map<std::string, std::function<boost::any (boost::archive::binary_iarchive&, size_t)>> DataParserMap;

    struct StoredString
    {
        std::string file;
        std::string id;
    };

    class ObjectTemplateReaderV2
    {
    public:
        explicit ObjectTemplateReaderV2(const std::shared_ptr<anh::resource::ResourceHandle>& resource);

        bool HasDerived() const;
        std::string GetDerivedName() const;
        std::string GetType() const;
        void ReadData(swganh::object::ObjectTemplate& output);

    private:
        IffReader iff_io_;
        std::string type_;
        std::string derived_name_;
        bool has_derived_;
    };

    class ObjectTemplateReader
    {
    public:        
        ObjectTemplateReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource, const DataParserMap& data_parsers);
    
        template<typename T>
        boost::optional<T> GetValue(const std::string& var_name)
        {
            boost::optional<T> value;
    
            auto find_iter = object_data_.find(var_name);
    
            if (find_iter == object_data_.end())
            {
                value = derived_->GetValue<T>(var_name);
            }
            else
            {
                value = boost::any_cast<T>(find_iter->second);
            }
    
            return value;
        }

        static boost::any ParseBool(boost::archive::binary_iarchive& archive, size_t size);
        static boost::any ParseInt(boost::archive::binary_iarchive& archive, size_t size);
        static boost::any ParseFloat(boost::archive::binary_iarchive& archive, size_t size);
        static boost::any ParseString(boost::archive::binary_iarchive& archive, size_t size);
        static boost::any ParseStoredString(boost::archive::binary_iarchive& archive, size_t size);

    private:
        std::unique_ptr<ObjectTemplateReader> LoadDerived_(anh::resource::ResourceManager* resource_manager);
    
        void LoadObjectData_();
    
        boost::any ParseData(const std::string& field_name, boost::archive::binary_iarchive& archive, size_t size);
        
        DataParserMap data_parsers_;
        std::unique_ptr<ObjectTemplateReader> derived_;
        IffReader iff_io_;
        std::unordered_map<std::string, boost::any> object_data_;
    };

}}}

#endif  // SWGANH_TRE_READERS_OBJECT_TEMPLATE_READER_H_
