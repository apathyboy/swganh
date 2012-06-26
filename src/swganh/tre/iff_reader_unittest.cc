// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

#include "anh/utilities.h"
#include "swganh/tre/iff_reader.h"

using swganh::tre::BadFileFormat;
using swganh::tre::IffReader;
using swganh::tre::IffReaderV2;

typedef boost::iostreams::stream<boost::iostreams::array_source> ResourceStream;

BOOST_AUTO_TEST_SUITE(SwganhTre)

BOOST_AUTO_TEST_CASE(ThrowsOnResourcesWithBadHeaders)
{
    std::stringstream ss;
    ss << "ABCD" << 1234;

    BOOST_CHECK_THROW(IffReader tmp(ss), IffReader::BadFileFormat);
}

BOOST_AUTO_TEST_CASE(AcceptsResourcesWithValidHeaders)
{
    uint32_t size = 0;

    std::stringstream ss;
    ss.write("FORM", 4);
    ss.write(reinterpret_cast<char*>(&size), sizeof(uint32_t));

    BOOST_CHECK_NO_THROW(IffReader tmp(ss));
}

BOOST_AUTO_TEST_SUITE_END()
    
BOOST_AUTO_TEST_SUITE(SwganhTreV2)

BOOST_AUTO_TEST_CASE(ThrowsOnResourcesWithBadHeaders)
{
    char tmp[8] = {    
        0x41, 0x42, 0x43, 0x44, 0x31, 0x32, 0x33, 0x34
    };

    std::vector<char> data(sizeof(tmp));

    std::copy(tmp, tmp+sizeof(tmp), std::begin(data));

    BOOST_CHECK_THROW(IffReaderV2 tmp(data), BadFileFormat);
}

BOOST_AUTO_TEST_CASE(AcceptsResourcesWithValidHeaders)
{
    char tmp[8] = {    
        0x46, 0x4f, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x00
    };

    std::vector<char> data(sizeof(tmp));

    std::copy(tmp, tmp+sizeof(tmp), std::begin(data));

    BOOST_CHECK_NO_THROW(IffReaderV2 tmp(data));
}

BOOST_AUTO_TEST_SUITE_END()