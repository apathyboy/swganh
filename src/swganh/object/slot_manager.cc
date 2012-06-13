

#include "slot_manager.h"
#include "object.h"

using swganh::object::Object;
using swganh::object::SlotManager;

SlotManager::SlotManager(Object* owner, const SlotDescriptor& slot_descriptor, const SlotArrangement& slot_arrangement)
    : owner_(owner)
    , slot_descriptor_(slot_descriptor)
    , slot_arrangement_(slot_arrangement)
{}
