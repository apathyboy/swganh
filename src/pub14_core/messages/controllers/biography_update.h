// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <cstdint>
#include <string>
#include "anh/byte_buffer.h"

#include "pub14_core/messages/obj_controller_message.h"

namespace swganh {
namespace messages {
namespace controllers {

    class BiographyUpdate : public ObjControllerMessage
    {
    public:
        explicit BiographyUpdate(uint32_t controller_type = 0x0000000B)
            : ObjControllerMessage(controller_type, message_type())
            , owner_id(0)
            , biography(L"")
        {}

        explicit BiographyUpdate(ObjControllerMessage controller_message)
            : ObjControllerMessage(std::move(controller_message))
        {
            OnControllerDeserialize(std::move(data));
        }

        static uint32_t message_type() { return 0x000001DB; }
        
        uint64_t owner_id;
        std::wstring biography;
        

        void OnControllerSerialize(anh::ByteBuffer& buffer) const
        {
            buffer.write(owner_id);
            buffer.write<std::wstring>(biography);
        }

        void OnControllerDeserialize(anh::ByteBuffer buffer)
        {
            owner_id = buffer.read<uint64_t>();
            biography = buffer.read<std::wstring>();
        }
    };

}}}  // namespace swganh::messages::controllers
