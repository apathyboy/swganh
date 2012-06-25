// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "anh/resource/resource_manager.h"

#include "slot_descriptor_reader.h"

using anh::resource::ResourceHandle;
using swganh::tre::readers::SlotDescriptorReader;


typedef boost::iostreams::stream<boost::iostreams::array_source> ResourceStream;

BOOST_AUTO_TEST_SUITE(SlotDescriptorReaderTests)

BOOST_AUTO_TEST_CASE(CanReadSlotDescriptors)
{
    char tmp[50] = {    
        0x46, 0x4f, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x2A, 0x53, 0x4C,
        0x54, 0x44, 0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x1E,
        0x30, 0x30, 0x30, 0x30, 0x44, 0x41, 0x54, 0x41, 0x00, 0x00,
        0x00, 0x12, 0x69, 0x6E, 0x76, 0x65, 0x6E, 0x74, 0x6F, 0x72,
        0x79, 0x00, 0x64, 0x61, 0x74, 0x61, 0x70, 0x61, 0x64, 0x00
    };
    
    auto data = std::make_shared<std::vector<char>>(sizeof(tmp));

    std::copy(tmp, tmp+50, std::begin(*data));

    auto resource = std::make_shared<ResourceHandle>(nullptr, "abstract/slot/descriptor/droid.iff", data);

    SlotDescriptorReader reader(resource);

    auto slots = reader.GetSlots();
        
    BOOST_CHECK_EQUAL(2, slots.size());
    BOOST_CHECK_EQUAL(std::string("inventory"), slots[0]);
    BOOST_CHECK_EQUAL(std::string("datapad"), slots[1]);
}

BOOST_AUTO_TEST_SUITE_END()
