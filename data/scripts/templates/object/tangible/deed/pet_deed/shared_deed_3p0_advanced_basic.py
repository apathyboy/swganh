#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *	

def create(kernel):
	result = Tangible()

	result.template = "object/tangible/deed/pet_deed/shared_deed_3p0_advanced_basic.iff"
	result.attribute_template_id = 2
	result.stfName("deed","p0_advanced_basic_deed")		
	
	#### BEGIN MODIFICATIONS ####
	result.setStringAttribute("radial_filename", "radials/deed_datapad.py")
	result.setStringAttribute("deed_pcd", "object/intangible/pet/shared_3po_protocol_droid_crafted.iff")
	result.setStringAttribute("deed_mobile", "object/mobile/shared_3po_protocol_droid_crafted.iff")
	####  END MODIFICATIONS  ####
	
	return result