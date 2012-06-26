
#include "slot_arrangement_reader.h"

#include "anh/resource/resource_manager.h"

using swganh::tre::IffReader;
using swganh::tre::readers::SlotArrangementReader;


SlotArrangementReader::SlotArrangementReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource)
    : iff_reader_(resource->GetBuffer())
{
    auto arg_node = iff_reader_.FindForm("ARGD");

    if (arg_node)
    {
        LoadArranagementFromNodes_(arg_node->FindForm("0000")->FindAllRecords("ARG "));
    }
    else
    {
        throw IffReader::BadFileFormat("File is not in the SLTD format: " + resource->GetName());
    }
}

SlotArrangementReader::~SlotArrangementReader()
{}

const std::vector<SlotArrangementReader::SlotList>& SlotArrangementReader::GetArrangement() const
{
    return arrangement_;
}

void SlotArrangementReader::LoadArranagementFromNodes_(std::list<IffReader::Node*>& nodes)
{
    for (auto& node : nodes)
    {
        arrangement_.push_back(LoadSlotsFromNode_(node));
    }
}

SlotArrangementReader::SlotList SlotArrangementReader::LoadSlotsFromNode_(IffReader::Node* node)
{
    SlotList slots;

    size_t read_size = 0, data_size = node->data.size();
    while (read_size < data_size)
    {
        std::string slot(&node->data[read_size]);
        read_size += slot.length() + 1;

        slots.push_back(slot);
    }

    return slots;
}
