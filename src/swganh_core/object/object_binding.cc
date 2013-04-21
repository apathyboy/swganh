// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef WIN32
#include <Python.h>
#endif

#include "swganh_core/object/object_binding.h"
#include "swganh_core/object/creature/creature_binding.h"
#include "swganh_core/object/player/player_binding.h"
#include "swganh_core/object/guild/guild_binding.h"
#include "swganh_core/object/tangible/tangible_binding.h"
#include "swganh_core/object/object_controller_binding.h"
#include "swganh_core/object/template_binding.h"

BOOST_PYTHON_MODULE(py_object) 
{
    docstring_options local_docstring_options(true, true, false);

    exportObject();
    exportTangible();
    exportCreature();
    exportPlayer();
	exportWeapon();
    exportObjectController();
    exportOutOfBand();
	exportObjectTemplate();
}