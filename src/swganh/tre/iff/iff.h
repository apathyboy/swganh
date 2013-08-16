// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <memory>
#include <string>

#include <swganh/byte_buffer.h>
#include <swganh/tre/visitors/visitor_interface.h>

namespace swganh {
namespace tre {

	class iff_file
	{
	public:

		static void loadIFF(swganh::ByteBuffer input, std::shared_ptr<VisitorInterface> visitor=nullptr);

	private:
		static bool isNodeNameCharacter_(char c);
		static unsigned int getNameSize_(char namedata[], unsigned int size);
		static bool isFolderNode_(std::string& name);
		static std::string getIFFName_(swganh::ByteBuffer& input);
	};

}}  // namespace swganh::tre
