// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_TRE_READERS_SLOT_DESCRIPTOR_READER_H_
#define SWGANH_TRE_READERS_SLOT_DESCRIPTOR_READER_H_

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

    class SlotDescriptorReader
    {
    public:
        explicit SlotDescriptorReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource);

        ~SlotDescriptorReader();

        const std::vector<std::string>& GetSlots() const;

    private:
        SlotDescriptorReader();

        void LoadSlotsFromNode_(IffReaderV2::Node* node);

        std::vector<std::string> slots_;
        IffReaderV2 iff_reader_;
    };

}}}

#endif  // SWGANH_TRE_READERS_SLOT_DESCRIPTOR_READER_H_
