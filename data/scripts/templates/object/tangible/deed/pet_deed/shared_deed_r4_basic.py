#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *	

def create(kernel):
	result = Tangible()

	result.template = "object/tangible/deed/pet_deed/shared_deed_r4_basic.iff"
	result.attribute_template_id = 2
	result.stfName("deed","r4_basic_deed")		
	
	#### BEGIN MODIFICATIONS ####
	result.setStringAttribute("radial_filename", "radials.deed_datapad")
	result.setStringAttribute("deed_pcd", "object/intangible/pet/shared_r4_crafted.iff")
	result.setStringAttribute("deed_mobile", "object/mobile/shared_r4_crafted.iff")
	####  END MODIFICATIONS  ####
	
	return result