// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <queue>
#include <tuple>

#include <swganh_core/map/map_service_interface.h>

#include <swganh_core/messages/get_map_locations_response_message.h>
#include <swganh_core/messages/get_map_locations_request_message.h>

namespace sql {
    class Connection;
}

namespace swganh {
namespace app {
	class SwganhKernel;
} // namespace swganh::app

namespace connection {
	class ConnectionClientInterface;
} // namespace swganh::connection

namespace database {
    class DatabaseManager;
}

namespace object {
	class Object;
} // namespace swganh::object

namespace simulation {
	class SimulationService;
} // namespace swganh::simulation

namespace map {

	class MapService : public MapServiceInterface
	{
	public:
		MapService(swganh::app::SwganhKernel* kernel);
		~MapService();

        virtual void Initialize();
		virtual void Startup();

		virtual void AddLocation(std::string scene, std::wstring name, float x, float z, uint32_t category, uint32_t sub_category);
		virtual void RemoveLocation(std::string scene, std::wstring name);
		virtual bool LocationExists(std::string scene, std::wstring name);

		virtual void AddLocation(uint32_t scene_id, std::wstring name, float x, float z, uint32_t category, uint32_t sub_category);
		virtual void RemoveLocation(uint32_t scene_id, std::wstring name);
		virtual bool LocationExists(uint32_t scene_id, std::wstring name);

	private:
		void InsertLocation(uint32_t scene_id, swganh::messages::MapLocation& location);
		void SyncAddLocation(uint32_t scene_id, swganh::messages::MapLocation& location);
		void SyncRemoveLocation(uint32_t scene_id, swganh::messages::MapLocation& location);

		void LoadStaticLocations();
		void LoadDynamicLocations();
		void LoadHighestLocationId();
        void AddLocationToStorage(const std::shared_ptr<sql::Connection>& connection, uint32_t scene_id, swganh::messages::MapLocation location);
        void RemoveLocationFromStorage(const std::shared_ptr<sql::Connection>& connection, uint32_t scene_id, swganh::messages::MapLocation location);

		void HandleRequestMapLocationsMessage(
			const std::shared_ptr<swganh::connection::ConnectionClientInterface>& client, 
			swganh::messages::GetMapLocationsRequestMessage* message);

		swganh::app::SwganhKernel* kernel_;
		swganh::simulation::SimulationService* simulation_;
		swganh::database::DatabaseManager* database_manager_;
		std::map<uint32_t, std::list<swganh::messages::MapLocation>> locations_;

		uint32_t next_location_id_;
	};

}} // swganh::map