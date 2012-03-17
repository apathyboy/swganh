
#include "anh/python_shared_ptr.h"
#include "swganh/object/object_binding.h"
#include "swganh/object/creature/creature_binding.h"
#include "swganh/object/player/player_binding.h"
#include "swganh/object/guild/guild_binding.h"
#include "swganh/object/tangible/tangible_binding.h"
#include "swganh/object/object_controller_binding.h"

BOOST_PYTHON_MODULE(py_object) 
{
    docstring_options local_docstring_options(true, true, false);

    exportObject();
    exportTangible();
    exportCreature();
    exportPlayer();
    exportObjectController();
    exportOutOfBand();
}