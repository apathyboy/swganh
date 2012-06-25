
#include "object_template_reader.h"

#include "anh/resource/resource_manager.h"

using swganh::tre::IffReader;
using swganh::tre::readers::ObjectTemplateReader;

ObjectTemplateReader::ObjectTemplateReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource)
    : iff_io_(resource->GetBufferStream())
{
    derived_ = LoadDerived_(resource->GetResourceManager());
    LoadObjectData_();
}

std::unique_ptr<ObjectTemplateReader> ObjectTemplateReader::LoadDerived_(anh::resource::ResourceManager* resource_manager)
{
    std::unique_ptr<ObjectTemplateReader> derived = nullptr;

    auto derv_node = iff_io_.FindNode("DERV");

    if (derv_node->data.size() > 0)
    {
        auto derived_resource = resource_manager->GetHandle(&derv_node->data[0]);

        if (derived_resource)
        {
            derived.reset(new ObjectTemplateReader(derived_resource));
        }
    }

    return derived;
}

void ObjectTemplateReader::LoadObjectData_()
{
    auto pcnt_nodes = iff_io_.FindAllNodes("PCNT");

    for (auto& pcnt_node : pcnt_nodes)
    {
        LoadObjectDataFromNode_(pcnt_node);
    }
}

void ObjectTemplateReader::LoadObjectDataFromNode_(IffReader::IffNode* node)
{
    auto xxxx_nodes = node->FindAllNodes("XXXX");

    for (auto& xxxx_node : xxxx_nodes)
    {
        std::string name(&xxxx_node->data[0]);

        size_t data_offset = name.length() + 1;

        boost::any data = ParseData(xxxx_node->data, data_offset);

        object_data_.insert(std::make_pair(name, std::move(data)));
    }
}

boost::any ObjectTemplateReader::ParseData(const std::vector<char>& data, size_t offset)
{
    return boost::any(std::string(""));
}
