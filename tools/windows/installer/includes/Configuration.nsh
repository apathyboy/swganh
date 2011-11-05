
var GalaxyNameId
var GalaxyName
var GalaxyAddressid
var GalaxyAddress
var DbHostId
var DbHost
var DbUserId
var DbUser
var DbPassId
var DbPass

Function Configuration
    !insertmacro MUI_HEADER_TEXT "Configure SWGANH Server" "Set the initial configuration values for this server installation."

    nsDialogs::Create 1018
  
    ${NSD_CreateLabel} 0 0 100% 30u "Set the configuration for this server installation."
  	
    ${NSD_CreateLabel} 0 40u 25% 12u "Galaxy Name"  	
    ${NSD_CreateText} 30% 40u 35% 12u "My SWG:ANH Galaxy"
    Pop $GalaxyNameId

    ${NSD_CreateLabel} 0 55u 25% 12u "Galaxy IP"  	
    ${NSD_CreateText} 30% 55u 35% 12u "127.0.0.1"
    Pop $GalaxyAddressid

    ${NSD_CreateLabel} 0 70u 25% 12u "Database Host"
    ${NSD_CreateText} 30% 70u 35% 12u "127.0.0.1"
    Pop $DbHostId

    ${NSD_CreateLabel} 0 85u 25% 12u "Database Username"
    ${NSD_CreateText} 30% 85u 35% 12u ""
    Pop $DbUserId

    ${NSD_CreateLabel} 0 100u 25% 12u "Database Password"
    ${NSD_CreatePassword} 30% 100u 35% 12u ""
    Pop $DbPassId
  
    nsDialogs::Show
FunctionEnd


Function ConfigurationValidate
    ${NSD_GetText} $GalaxyNameId $GalaxyName
    ${NSD_GetText} $GalaxyAddressid $GalaxyAddress
    ${NSD_GetText} $DbHostId $DbHost
    ${NSD_GetText} $DbUserId $DbUser
    ${NSD_GetText} $DbPassId $DbPass
FunctionEnd

