// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_OBJECT_SLOT_MANAGER_H_
#define SWGANH_OBJECT_SLOT_MANAGER_H_

#include <memory>
#include <set>
#include <string>
#include <unordered_map>

namespace swganh {
namespace object {

    class Object;

    class SlotManager
    {
    public:
        typedef std::set<std::string> SlotDescriptor;
        typedef std::set<std::string> ArrangementGroup;
        typedef std::set<ArrangementGroup> SlotArrangement;
   
        SlotManager(swganh::object::Object* owner, const SlotDescriptor& slot_descriptor, const SlotArrangement& arrangement);
        
        
    private:
        SlotManager();
        
        swganh::object::Object* owner_;
        const SlotDescriptor& slot_descriptor_;
        const SlotArrangement& slot_arrangement_;
    };

}}

#endif  // SWGANH_OBJECT_SLOT_MANAGER_H_

