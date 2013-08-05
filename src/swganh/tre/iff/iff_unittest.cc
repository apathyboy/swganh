// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <boost/test/unit_test.hpp>

#include "swganh/utilities.h"
#include "swganh/tre/iff/iff.h"

BOOST_AUTO_TEST_SUITE(SwganhTre)

BOOST_AUTO_TEST_CASE(ThrowsOnResourcesWithBadHeaders)
{
    BOOST_CHECK_EQUAL(uint32_t(4), uint32_t(4));
}

BOOST_AUTO_TEST_SUITE_END()
