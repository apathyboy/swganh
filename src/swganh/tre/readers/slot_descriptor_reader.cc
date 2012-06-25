
#include "slot_descriptor_reader.h"

#include "anh/resource/resource_manager.h"

using swganh::tre::IffReader;
using swganh::tre::readers::SlotDescriptorReader;

SlotDescriptorReader::SlotDescriptorReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource)
    : iff_reader_(resource->GetBufferStream())
{
    auto sltd_node = iff_reader_.FindNode("SLTDFORM");

    if (!sltd_node)
    {
        throw IffReader::InvalidFormType("File is not in the SLTD format: " + resource->GetName());
    }

    LoadSlotsFromNode_(sltd_node);
}

SlotDescriptorReader::~SlotDescriptorReader()
{}

const std::vector<std::string>& SlotDescriptorReader::GetSlots() const
{
    return slots_;
}

void SlotDescriptorReader::LoadSlotsFromNode_(IffReader::IffNode* node)
{
    size_t read_size = 0, data_size = node->data.size();
    while (read_size < data_size)
    {
        std::string slot(&node->data[read_size]);
        read_size += slot.length() + 1;

        slots_.push_back(slot);
    }
}
