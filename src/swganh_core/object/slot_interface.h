// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <memory>
#include <functional>

namespace swganh {
namespace object {

	class Object;

	class SlotInterface
	{
	public:

        typedef std::function<void(const std::shared_ptr<Object>&)> ViewWalkerFunction;

        virtual ~SlotInterface() {}

		virtual std::shared_ptr<Object> insert_object(std::shared_ptr<Object> insertObject) = 0;
		virtual void remove_object(const std::shared_ptr<Object>& removeObject) = 0;
		virtual void view_objects(ViewWalkerFunction walkerFunction) = 0;
		virtual bool is_filled() = 0;
	};

}}
