// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include "swganh_core/object/slot_interface.h"

namespace swganh {
namespace object {

	class SlotExclusive : public SlotInterface
	{
	public:

        virtual ~SlotExclusive() {}

		virtual std::shared_ptr<Object> insert_object(std::shared_ptr<Object> insertObject);
		virtual void remove_object(const std::shared_ptr<Object>& removeObject);
		virtual bool is_filled() { return held_object_ != nullptr; }
		virtual void view_objects(ViewWalkerFunction walkerFunction);

	private:
		std::shared_ptr<Object> held_object_;
	};

}
}
