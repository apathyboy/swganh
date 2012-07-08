
#include "object_template_manager.h"

#include "anh/resource/resource_manager.h"

#include "swganh/tre/readers/object_template_reader.h"

#include "object_template.h"

using namespace swganh::object;
using swganh::tre::readers::ObjectTemplateReaderV2;


void ObjectTemplateFactory::AddTemplateCreator(std::string name, ObjectTemplateCreator&& creator)
{
    auto find_iter = object_template_creators_.find(name);

    if (find_iter == std::end(object_template_creators_))
    {
        object_template_creators_.insert(find_iter, std::make_pair(name, std::move(creator)));
    }
    else
    {
        find_iter->second = std::move(creator);
    }
}

void ObjectTemplateFactory::RemoveTemplateCreator(const std::string& name)
{
    object_template_creators_.erase(name);
}

std::unique_ptr<ObjectTemplate> ObjectTemplateFactory::CreateObjectTemplate(const std::string& name)
{
    auto find_iter = object_template_creators_.find(name);

    if (find_iter != std::end(object_template_creators_))
    {
        return find_iter->second(name);
    }

    return nullptr;
}


ObjectTemplateManager::ObjectTemplateManager(anh::resource::ResourceManager* resource_manager)
    : resource_manager_(resource_manager)
{}

ObjectTemplate* ObjectTemplateManager::GetObjectTemplate(const std::string& name)
{
    ObjectTemplate* object_template = nullptr;

    auto find_iter = object_templates_.find(name);

    if (find_iter == std::end(object_templates_))
    {
        auto new_object_template = CreateObjectTemplate(name);

        object_template = new_object_template.get();

        object_templates_.insert(find_iter, std::make_pair(name, std::move(new_object_template)));
    }
    else
    {
        object_template = find_iter->second.get();
    }

    return object_template;
}

std::unique_ptr<ObjectTemplate> ObjectTemplateManager::CreateObjectTemplate(const std::string& name)
{
    auto resource = resource_manager_->GetHandle(name);

    if (!resource)
    {
        return nullptr;
    }
    
    ObjectTemplateReaderV2 reader(resource);

    auto new_template = object_template_factory_.CreateObjectTemplate(reader.GetType());
    reader.ReadData(*new_template);

    if (reader.HasDerived())
    {
        new_template->SetDerived(GetObjectTemplate(reader.GetDerivedType()));
    }

    return new_template;
}