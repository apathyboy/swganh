// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "anh/utilities.h"
#include "swganh/tre/iff_reader.h"

using swganh::tre::IffReader;

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
