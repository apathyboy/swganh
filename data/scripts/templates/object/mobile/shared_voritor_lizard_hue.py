#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *	

def create(kernel):
	result = Creature()

	result.template = "object/mobile/shared_voritor_lizard_hue.iff"
	result.attribute_template_id = 9
	result.stfName("monster_name","voritor_lizard")		
	
	#### BEGIN MODIFICATIONS ####
	result.setStringAttribute("radial_filename", "radials/player_pet.py")
	result.options_mask = 0x100
	result.pvp_status = PVPSTATUS.PvPStatus_None
	####  END MODIFICATIONS  ####
	
	return result