// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_OBJECT_OBJECT_TEMPLATE_MANAGER_H_
#define SWGANH_OBJECT_OBJECT_TEMPLATE_MANAGER_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace anh {
namespace resource {
    class ResourceManager;
}}

namespace swganh {
namespace object {

    class ObjectTemplate;
    
    typedef std::function<std::unique_ptr<ObjectTemplate> ()> ObjectTemplateCreator;

    class ObjectTemplateFactory
    {
    public:
        void AddTemplateCreator(std::string name, ObjectTemplateCreator&& creator);
        void RemoveTemplateCreator(const std::string& name);
        std::unique_ptr<ObjectTemplate> CreateObjectTemplate(const std::string& name);
        
    private:
        typedef std::unordered_map<std::string, ObjectTemplateCreator> ObjectTemplateCreatorMap;
        ObjectTemplateCreatorMap object_template_creators_;
    };

    class ObjectTemplateManager
    {
    public:
        ObjectTemplateManager(anh::resource::ResourceManager* resource_manager);

        ObjectTemplate* GetObjectTemplate(const std::string& name);

    private:
        typedef std::unordered_map<std::string, std::unique_ptr<ObjectTemplate>> ObjectTemplateMap;
        
        ObjectTemplate* CreateObjectTemplate(const std::string& name);

        anh::resource::ResourceManager* resource_manager_;
        ObjectTemplateMap object_templates_;
        ObjectTemplateFactory object_template_factory_;
    };

}}

#endif
