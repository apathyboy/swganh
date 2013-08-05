// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <boost/test/unit_test.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include "swganh/utilities.h"
#include "swganh/tre/iff/iff.h"

using swganh::tre::IffReader;

typedef boost::iostreams::stream<boost::iostreams::array_source> ResourceStream;
    
BOOST_AUTO_TEST_SUITE(SwganhTre)

BOOST_AUTO_TEST_CASE(ThrowsOnResourcesWithBadHeaders)
{
    char tmp[8] = {    
        0x41, 0x42, 0x43, 0x44, 0x31, 0x32, 0x33, 0x34
    };

    std::vector<char> data(sizeof(tmp));

    std::copy(tmp, tmp+sizeof(tmp), std::begin(data));

    BOOST_CHECK_THROW(IffReader tmp(data), IffReader::BadFileFormat);
}

BOOST_AUTO_TEST_CASE(AcceptsResourcesWithValidHeaders)
{
    char tmp[8] = {    
        0x46, 0x4f, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x00
    };

    std::vector<char> data(sizeof(tmp));

    std::copy(tmp, tmp+sizeof(tmp), std::begin(data));

    BOOST_CHECK_NO_THROW(IffReader tmp(data));
}

BOOST_AUTO_TEST_SUITE_END()