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

namespace anh {
namespace resource {
    class ResourceHandle;
    class ResourceManager;
}}

namespace swganh {
namespace tre {
namespace readers {

    class ObjectTemplateReader
    {
    public:        
        explicit ObjectTemplateReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource);
    
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
    
    private:
        std::unique_ptr<ObjectTemplateReader> LoadDerived_(anh::resource::ResourceManager* resource_manager);
    
        void LoadObjectData_();
    
        void LoadObjectDataFromNode_(IffReader::IffNode* node);
    
        boost::any ParseData(const std::vector<char>& data, size_t offset);
    
        std::unique_ptr<ObjectTemplateReader> derived_;
        IffReader iff_io_;
        std::unordered_map<std::string, boost::any> object_data_;
    };

}}}

#endif  // SWGANH_TRE_READERS_OBJECT_TEMPLATE_READER_H_
