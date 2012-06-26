// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "slot_descriptor_reader.h"

#include "anh/resource/resource_manager.h"

using swganh::tre::IffReader;
using swganh::tre::readers::SlotDescriptorReader;

SlotDescriptorReader::SlotDescriptorReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource)
    : iff_reader_(resource->GetBuffer())
{
    auto sltd_node = iff_reader_.Form("SLTD");

    if (!sltd_node)
    {
        throw IffReader::BadFileFormat("File is not in the SLTD format: " + resource->GetName());
    }

    LoadSlotsFromNode_(sltd_node->Form("0000")->Record("DATA"));
}

SlotDescriptorReader::~SlotDescriptorReader()
{}

const std::vector<std::string>& SlotDescriptorReader::GetSlots() const
{
    return slots_;
}

void SlotDescriptorReader::LoadSlotsFromNode_(IffReader::Node* node)
{
    size_t read_size = 0, data_size = node->size;
    while (read_size < data_size)
    {
        std::string slot(&node->data[read_size]);
        read_size += slot.length() + 1;

        slots_.push_back(slot);
    }
}
