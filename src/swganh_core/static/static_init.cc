// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include "static_init.h"

#include "swganh/logger.h"

#include "swganh/plugin/bindings.h"
#include "swganh/plugin/plugin_manager.h"

#include "swganh/app/swganh_kernel.h"

#include "static_service.h"
#include "version.h"

namespace swganh {
namespace statics {

void Initialize(swganh::app::SwganhKernel* kernel) 
{    
    swganh::plugin::ObjectRegistration registration;
    registration.version.major = VERSION_MAJOR;
    registration.version.minor = VERSION_MINOR;
    
    // Register Static Service
	{ // Static::StaticService
        registration.CreateObject = [kernel] (swganh::plugin::ObjectParams* params) -> void * {
            return new StaticService(kernel);;
        };

        registration.DestroyObject = [] (void * object) {
            if (object) {
                delete static_cast<StaticService*>(object);
            }
        };

        kernel->GetPluginManager()->RegisterObject("Static::StaticService", &registration);
	}
}

}}  // namespace swganh::static
