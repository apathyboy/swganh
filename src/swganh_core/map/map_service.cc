// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#include <swganh_core/map/map_service.h>

#include <swganh/app/swganh_kernel.h>
#include <swganh/event_dispatcher.h>
#include <swganh/service/service_manager.h>
#include <swganh/database/database_manager.h>
#include <swganh/logger.h>

#include <swganh_core/simulation/simulation_service.h>
#include <swganh_core/connection/connection_service_interface.h>
#include <swganh_core/connection/connection_client_interface.h>

#include <cppconn/exception.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/sqlstring.h>

#include <swganh_core/messages/get_map_locations_request_message.h>


using namespace swganh::map;

using swganh::connection::ConnectionServiceInterface;
using swganh::service::ServiceDescription;
using swganh::simulation::SimulationService;
using swganh::connection::ConnectionClientInterface;
using swganh::messages::MapLocation;
using swganh::messages::GetMapLocationsRequestMessage;
using swganh::messages::GetMapLocationsResponseMessage;

MapService::MapService(swganh::app::SwganhKernel* kernel)
	: kernel_(kernel)
{
    SetServiceDescription(ServiceDescription(
		"MapService",
		"map",
		"0.1",
		"127.0.0.1",
		0,
		0,
		0
		));
}

MapService::~MapService()
{}

void MapService::Initialize()
{
	simulation_ = kernel_->GetServiceManager()->GetService<SimulationService>("SimulationService");
    database_manager_ = kernel_->GetDatabaseManager();
}

void MapService::Startup()
{
	LoadStaticLocations();
	LoadDynamicLocations();
	LoadHighestLocationId();

	// Register message handler.
	auto connection_service = kernel_->GetServiceManager()->GetService<ConnectionServiceInterface>("ConnectionService");
	connection_service->RegisterMessageHandler(&MapService::HandleRequestMapLocationsMessage, this);
}

void MapService::AddLocation(std::string scene, std::wstring name, float x, float z, uint32_t category, uint32_t sub_category)
{
	AddLocation(simulation_->SceneIdByName(scene), name, x, z, category, sub_category);
}

void MapService::RemoveLocation(std::string scene, std::wstring name)
{
	RemoveLocation(simulation_->SceneIdByName(scene), name);
}

bool MapService::LocationExists(std::string scene, std::wstring name)
{
	return LocationExists(simulation_->SceneIdByName(scene), name);
}

void MapService::AddLocation(uint32_t scene_id, std::wstring name, float x, float z, uint32_t category, uint32_t sub_category)
{
	auto i = locations_.find(scene_id);
	if(i == locations_.end())
		return;

	MapLocation new_loc;
	new_loc.id = next_location_id_++;
	new_loc.name = name;
	new_loc.x = x;
	new_loc.y = z;
	new_loc.type_displayAsCategory = category;
	new_loc.type_displayAsSubcategory = sub_category;
	i->second.push_back(new_loc);

    database_manager_->ExecuteAsync(std::bind(&MapService::AddLocationToStorage, this, std::placeholders::_1, scene_id, new_loc), "galaxy");
}

void MapService::RemoveLocation(uint32_t scene_id, std::wstring name)
{
	auto i = locations_.find(scene_id);
	if(i == locations_.end())
		return;

	for(auto iter = i->second.begin(); iter != i->second.end(); iter++)
	{
		if(iter->name == name) {
            database_manager_->ExecuteAsync(std::bind(&MapService::RemoveLocationFromStorage, this, std::placeholders::_1, scene_id, *iter), "galaxy");
			iter = i->second.erase(iter);
		}
	}
}

bool MapService::LocationExists(uint32_t scene_id, std::wstring name)
{
	auto i = locations_.find(scene_id);
	if(i == locations_.end())
		return false;

	for(auto iter = i->second.begin(); iter != i->second.end(); iter++)
	{
		if(iter->name == name)
			return true;
	}

	return false;
}

void MapService::InsertLocation(uint32_t scene_id, MapLocation& location)
{
	auto i = locations_.find(scene_id);
	if(i == locations_.end())
	{
		locations_.insert(std::make_pair(scene_id, std::list<MapLocation>()));
	}

	locations_.at(scene_id).push_back(location);
}

void MapService::LoadStaticLocations()
{
	try
	{
		auto conn = kernel_->GetDatabaseManager()->getConnection("swganh_static");
		auto statement = std::shared_ptr<sql::Statement>(conn->createStatement());
		auto result = std::shared_ptr<sql::ResultSet>(statement->executeQuery("CALL sp_GetMapLocations"));

		while(result->next())
		{
			MapLocation location;
			location.id = result->getUInt("id");
			auto name = result->getString("name").asStdString();
			location.name = std::wstring(name.begin(), name.end());
			location.x = (float)result->getDouble("x");
			location.y = (float)result->getDouble("y");
			location.type_displayAsCategory = result->getUInt("category_main");
			location.type_displayAsSubcategory = result->getUInt("category_sub");

			uint32_t scene_id = result->getUInt("scene_id") + 1;
			InsertLocation(scene_id, location);
		}
	}
	catch(sql::SQLException &e) {
		LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
		LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
	}
}

void MapService::LoadDynamicLocations()
{
	try
	{
		auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");
		auto statement = std::shared_ptr<sql::Statement>(conn->createStatement());
		auto result = std::shared_ptr<sql::ResultSet>(statement->executeQuery("CALL sp_GetMapLocations"));

		while(result->next())
		{
			MapLocation location;
			location.id = result->getUInt("id");
			auto name = result->getString("name").asStdString();
			location.name = std::wstring(name.begin(), name.end());
			location.x = (float)result->getDouble("x");
			location.y = (float)result->getDouble("y");
			location.type_displayAsCategory = result->getUInt("category_main");
			location.type_displayAsSubcategory = result->getUInt("category_sub");

			uint32_t scene_id = result->getUInt("scene_id") + 1;
			InsertLocation(scene_id, location);
		}
	}
	catch(sql::SQLException &e) {
		LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
		LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
	}
}

void MapService::LoadHighestLocationId()
{
	try 
	{
		auto conn = kernel_->GetDatabaseManager()->getConnection("galaxy");
		auto statement = std::shared_ptr<sql::Statement>(conn->createStatement());
		auto result = std::shared_ptr<sql::ResultSet>(statement->executeQuery("CALL sp_GetHighestLocationId();"));

		while(result->next())
			next_location_id_ = result->getUInt(1);
	}
	catch(sql::SQLException &e) {
		LOG(error) << "SQLException at " << __FILE__ << " (" << __LINE__ << ": " << __FUNCTION__ << ")";
		LOG(error) << "MySQL Error: (" << e.getErrorCode() << ": " << e.getSQLState() << ") " << e.what();
	}
}

void MapService::AddLocationToStorage(const std::shared_ptr<sql::Connection>& connection, uint32_t scene_id, swganh::messages::MapLocation location)
{
    auto statement = connection->prepareStatement("CALL sp_UpdateLocation(?, ?, ?, ?, ?, ?, ?);");

    statement->setUInt64(1, location.id);
    statement->setString(2, std::string(location.name.begin(), location.name.end()));
    statement->setUInt(3, scene_id - 1);
    statement->setUInt(4, location.type_displayAsCategory);
    statement->setUInt(5, location.type_displayAsSubcategory);
    statement->setDouble(6, location.x);
    statement->setDouble(7, location.y);

    statement->execute();
}

void MapService::RemoveLocationFromStorage(const std::shared_ptr<sql::Connection>& connection, uint32_t scene_id, swganh::messages::MapLocation location)
{
    auto statement = connection->prepareStatement("CALL sp_RemoveLocation(?, ?, ?);");
	
    statement->setUInt64(1, location.id);
    statement->setUInt(2, scene_id - 1);
    statement->setString(3, std::string(location.name.begin(), location.name.end()));

    statement->execute();
}

void MapService::HandleRequestMapLocationsMessage(
	const std::shared_ptr<ConnectionClientInterface>& client, 
	swganh::messages::GetMapLocationsRequestMessage* message)
{
	swganh::messages::GetMapLocationsResponseMessage response;
	response.planet_name = message->planet_name;
	response.locations = locations_.find(simulation_->SceneIdByName(message->planet_name))->second;
	client->GetController()->Notify(&response);
}