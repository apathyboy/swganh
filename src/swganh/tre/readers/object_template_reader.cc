
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

ObjectTemplateReader::ObjectTemplateReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource)
    : iff_io_(resource->GetBuffer())
    , has_derived_(false)
{}

bool ObjectTemplateReader::HasDerived() const
{
    return has_derived_;
}

const std::string& ObjectTemplateReader::GetDerivedType() const
{
    return derived_name_;
}

const std::string& ObjectTemplateReader::GetType() const
{
    return type_;
}

void ObjectTemplateReader::ReadData(swganh::object::ObjectTemplate& output)
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

void ObjectTemplateReader::ReadType()
{
    type_ = iff_io_.Head()->name;
}

void ObjectTemplateReader::ReadDerived()
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