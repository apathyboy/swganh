// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#pragma once

#include "swganh_core/object/installation/installation_message_builder.h"

namespace swganh {
namespace object {

	class HarvesterInstallation;
    class HarvesterInstallationMessageBuilder : public swganh::object::InstallationMessageBuilder
    {
    public:
        HarvesterInstallationMessageBuilder(swganh::EventDispatcher* dispatcher) :
            InstallationMessageBuilder(dispatcher)
        {
            RegisterEventHandlers();
        }

        // baselines
        static boost::optional<swganh::messages::BaselinesMessage> BuildBaseline3(const std::shared_ptr<HarvesterInstallation>& harvester_installation);
        static boost::optional<swganh::messages::BaselinesMessage> BuildBaseline6(const std::shared_ptr<HarvesterInstallation>& harvester_installation);
		static boost::optional<swganh::messages::BaselinesMessage> BuildBaseline7(const std::shared_ptr<HarvesterInstallation>& harvester_installation);

    private:
        typedef swganh::ValueEvent<std::shared_ptr<HarvesterInstallation>> HarvesterInstallationEvent;

        void RegisterEventHandlers();
    };

}}  // swganh::object
