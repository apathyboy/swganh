// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "anh/resource/resource_manager.h"

#include "slot_arrangement_reader.h"

using anh::resource::ResourceHandle;
using swganh::tre::readers::SlotArrangementReader;


typedef boost::iostreams::stream<boost::iostreams::array_source> ResourceStream;

BOOST_AUTO_TEST_SUITE(SlotArrangementReaderTests)

BOOST_AUTO_TEST_CASE(CanReadSlotArrangements)
{
    char tmp[40] = {    
        0x46, 0x4f, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x20, 0x41, 0x52,
        0x47, 0x44, 0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x14,
        0x30, 0x30, 0x30, 0x30, 0x41, 0x52, 0x47, 0x20, 0x00, 0x00,
        0x00, 0x08, 0x64, 0x61, 0x74, 0x61, 0x70, 0x61, 0x64, 0x00
    };
    
    auto data = std::make_shared<std::vector<char>>(sizeof(tmp));

    std::copy(tmp, tmp+40, std::begin(*data));

    auto resource = std::make_shared<ResourceHandle>(nullptr, "abstract/slot/arrangement/arrangement_datapad.iff", data);

    SlotArrangementReader reader(resource);

    auto arrangement = reader.GetArrangement();
        
    BOOST_CHECK_EQUAL(1, arrangement.size());
    BOOST_CHECK_EQUAL(std::string("datapad"), arrangement[0][0]);
}

BOOST_AUTO_TEST_SUITE_END()
