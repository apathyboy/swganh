// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_TRE_READERS_SLOT_ARRANGEMENT_READER_H_
#define SWGANH_TRE_READERS_SLOT_ARRANGEMENT_READER_H_

#include <memory>
#include <string>
#include <vector>

#include "swganh/tre/iff_reader.h"

namespace anh {
namespace resource {
    class ResourceHandle;
}}

namespace swganh {
namespace tre {
namespace readers {

    class SlotArrangementReader
    {
    public:
        typedef std::vector<std::string> SlotList;

        explicit SlotArrangementReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource);

        ~SlotArrangementReader();

        const std::vector<SlotList>& GetArrangement() const;

    private:
        SlotArrangementReader();

        void LoadArranagementFromNodes_(std::list<IffReader::Node*>& nodes);
        SlotList LoadSlotsFromNode_(IffReader::Node* node);

        std::vector<SlotList> arrangement_;
        IffReader iff_reader_;
    };

}}}

#endif  // SWGANH_TRE_READERS_SLOT_ARRANGEMENT_READER_H_
