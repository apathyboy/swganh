// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "anh/resource/resource_manager.h"

#include "object_template_reader.h"

using anh::resource::ResourceHandle;
using swganh::tre::readers::ObjectTemplateReader;


typedef boost::iostreams::stream<boost::iostreams::array_source> ResourceStream;

BOOST_AUTO_TEST_SUITE(ObjectTemplateReaderTests)

BOOST_AUTO_TEST_CASE(CanReadObjectTemplate)
{
}

BOOST_AUTO_TEST_SUITE_END()
