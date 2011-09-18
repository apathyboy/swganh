/*
 This file is part of SWGANH. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2011 The SWG:ANH Team

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SWGANH_CHARACTER_CHARACTER_SERVICE_H_
#define SWGANH_CHARACTER_CHARACTER_SERVICE_H_

#include <cstdint>
#include <tuple>
#include <vector>

#include "swganh/base/base_service.h"

#include "swganh/character/character_data.h"
#include "swganh/character/messages/select_character.h"
#include "swganh/character/messages/delete_character_message.h"
#include "swganh/character/messages/client_create_character.h"
#include "swganh/character/messages/client_create_character_success.h"
#include "swganh/character/messages/client_create_character_failed.h"
#include "swganh/character/messages/client_random_name_request.h"
#include "swganh/character/messages/client_random_name_response.h"

namespace anh {
namespace app {
class KernelInterface;
}}  // namespace anh::app

namespace anh { namespace database { class DatabaseManagerInterface; } }

namespace swganh {
namespace connection {
struct ConnectionClient;
}}  // namespace swganh::connection

namespace swganh {
namespace login {
struct LoginClient;
}}  // namespace swganh::login

namespace swganh {
namespace character {
    
class CharacterService : public swganh::base::BaseService {
public:
    explicit CharacterService(std::shared_ptr<anh::app::KernelInterface> kernel);
    ~CharacterService();
    
    anh::service::ServiceDescription GetServiceDescription();

    void onStart();
    void onStop();

    void subscribe();

    // CharacterService API Methods

    std::vector<swganh::character::CharacterData> GetCharactersForAccount(uint64_t account_id);
    swganh::character::CharacterLoginData GetLoginCharacter(uint64_t character_id, uint64_t account_id);
    bool DeleteCharacter(uint64_t character_id, uint64_t account_id);
    std::wstring GetRandomNameRequest(const std::string& base_model);
    std::tuple<uint64_t, std::string> CreateCharacter(const swganh::character::messages::ClientCreateCharacter& character_info, uint32_t account_id);
    uint16_t GetMaxCharacters(uint64_t player_id);
private:
    void HandleSelectCharacter_(std::shared_ptr<swganh::connection::ConnectionClient> client, const swganh::character::messages::SelectCharacter& message);
    void HandleClientRandomNameRequest_(std::shared_ptr<swganh::connection::ConnectionClient> client, const swganh::character::messages::ClientRandomNameRequest& message);
    void HandleClientCreateCharacter_(std::shared_ptr<swganh::connection::ConnectionClient> client, const swganh::character::messages::ClientCreateCharacter& message);
    void HandleDeleteCharacterMessage_(
        std::shared_ptr<swganh::login::LoginClient> login_client, 
        const swganh::character::messages::DeleteCharacterMessage& message);

    // helpers
    std::string parseBio_(const std::string& bio);
    std::string parseHair_(const std::string& hair_model, const std::string& hair_customization);
    std::string setCharacterCreateErrorCode_(uint32_t error_id);
};

}}  // namespace swganh::character

#endif  // SWGANH_CHARACTER_CHARACTER_SERVICE_H_
