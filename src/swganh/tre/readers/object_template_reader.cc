
#include "object_template_reader.h"

#pragma warning( push )
#pragma warning( disable : 4244 )
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_iarchive.hpp>
#pragma warning( pop )

#include "anh/utilities.h"
#include "anh/resource/resource_manager.h"

#include "swganh/object/object_template.h"

namespace bs = boost::serialization;

using boost::archive::binary_iarchive;
using boost::iostreams::basic_array_source;
using boost::iostreams::stream_buffer;
using swganh::tre::IffReader;
using swganh::tre::readers::ObjectTemplateReader;
using swganh::tre::readers::ObjectTemplateReaderV2;
using swganh::tre::readers::DataParserMap;

ObjectTemplateReaderV2::ObjectTemplateReaderV2(const std::shared_ptr<anh::resource::ResourceHandle>& resource)
    : iff_io_(resource->GetBuffer())
    , has_derived_(false)
{}

bool ObjectTemplateReaderV2::HasDerived() const
{
    return has_derived_;
}

const std::string& ObjectTemplateReaderV2::GetDerivedType() const
{
    return derived_name_;
}

const std::string& ObjectTemplateReaderV2::GetType() const
{
    return type_;
}

void ObjectTemplateReaderV2::ReadData(swganh::object::ObjectTemplate& output)
{
    auto xxxx_nodes = iff_io_.FindAllRecords("XXXX");

    for (auto& xxxx_node : xxxx_nodes)
    {
        std::string name(&xxxx_node->data[0]);

        size_t data_offset = name.length() + 1;
        size_t remaining = xxxx_node->data.size() - data_offset;
        
        auto property = output.GetProperty(name);
        if (property)
        {
            stream_buffer<basic_array_source<char>> buffer(&xxxx_node->data[data_offset], remaining);
            binary_iarchive archive(buffer, boost::archive::no_header);

            property->Deserialize(archive, remaining);
        }
    }
}

void ObjectTemplateReaderV2::ReadType()
{
    type_ = iff_io_.Head()->name;
}

void ObjectTemplateReaderV2::ReadDerived()
{
    try
    {        
        auto derv_node = iff_io_.Form("DERV")->Record("XXXX");

        if (derv_node->data.size() > 0)
        {
            derived_name_ = &derv_node->data[0];
            has_derived_ = true;
        }
    }
    catch(const IffReader::InvalidFormType&)
    {}
}


ObjectTemplateReader::ObjectTemplateReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource, const DataParserMap& data_parsers)
    : iff_io_(resource->GetBuffer())
    , data_parsers_(data_parsers)
{
    derived_ = LoadDerived_(resource->GetResourceManager());
    LoadObjectData_();
}

std::unique_ptr<ObjectTemplateReader> ObjectTemplateReader::LoadDerived_(anh::resource::ResourceManager* resource_manager)
{
    std::unique_ptr<ObjectTemplateReader> derived = nullptr;

    try {
        auto derv_node = iff_io_.Form("DERV")->Record("XXXX");

        if (derv_node->data.size() > 0)
        {
            auto derived_resource = resource_manager->GetHandle(&derv_node->data[0]);

            if (derived_resource)
            {
                derived.reset(new ObjectTemplateReader(derived_resource, data_parsers_));
            }
        }
    } catch(const IffReader::InvalidFormType&)
    { // Mute any InvalidFormType that is thrown here as the DERV is optional
    }

    return derived;
}

void ObjectTemplateReader::LoadObjectData_()
{
    auto xxxx_nodes = iff_io_.FindAllRecords("XXXX");

    for (auto& xxxx_node : xxxx_nodes)
    {
        std::string name(&xxxx_node->data[0]);

        size_t data_offset = name.length() + 1;
        size_t remaining = xxxx_node->data.size() - data_offset;
        
        stream_buffer<basic_array_source<char>> buffer(&xxxx_node->data[data_offset], remaining);
        binary_iarchive archive(buffer, boost::archive::no_header);
    
        boost::any data = ParseData(name, archive, remaining);

        if (!data.empty())
        {
            object_data_.insert(std::make_pair(name, std::move(data)));
        }
    }
}

boost::any ObjectTemplateReader::ParseData(const std::string& field_name, boost::archive::binary_iarchive& archive, size_t size)
{
    auto find_iter = data_parsers_.find(field_name);

    boost::any parsed_data;

    if (find_iter != std::end(data_parsers_))
    {
        parsed_data = find_iter->second(archive, size);
    }

    return parsed_data;
}


boost::any ObjectTemplateReader::ParseBool(boost::archive::binary_iarchive& archive, size_t size)
{
    uint8_t enabled;
    boost::any parsed_data;

    archive >> enabled;

    if (enabled > 0)
    {
        uint8_t value;
        archive >> value;

        if (value > 0)
        {
            parsed_data = true;
        }
        else
        {
            parsed_data = false;
        }
    }

    return parsed_data;
}

boost::any ObjectTemplateReader::ParseInt(boost::archive::binary_iarchive& archive, size_t size)
{
    uint8_t enabled;
    boost::any parsed_data;

    archive >> enabled;

    if (enabled > 0)
    {
        uint8_t spacer;
        archive >> spacer;

        uint32_t value;
        archive >> value;

        parsed_data = anh::bigToHost(value);
    }

    return parsed_data;
}

boost::any ObjectTemplateReader::ParseFloat(boost::archive::binary_iarchive& archive, size_t size)
{
    uint8_t enabled;
    boost::any parsed_data;

    archive >> enabled;

    if (enabled > 0)
    {
        uint8_t spacer;
        archive >> spacer;

        float value;
        archive >> value;

        parsed_data = value;
    }

    return parsed_data;
}

boost::any ObjectTemplateReader::ParseString(boost::archive::binary_iarchive& archive, size_t size)
{
    uint8_t enabled;
    boost::any parsed_data;

    archive >> enabled;

    if (enabled > 0)
    {
        std::string value;
        archive >> value;

        parsed_data = value;
    }

    return parsed_data;
}

boost::any ObjectTemplateReader::ParseStoredString(boost::archive::binary_iarchive& archive, size_t size)
{
    uint8_t enabled;
    boost::any parsed_data;

    archive >> enabled;

    if (enabled > 0)
    {
        StoredString value;
        uint8_t spacer;

        archive >> spacer;
        archive >> value.file;

        archive >> spacer;
        archive >> value.id;

        parsed_data = value;
    }

    return parsed_data;
}
