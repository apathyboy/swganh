#### NOTICE: THIS FILE IS AUTOGENERATED
#### MODIFICATIONS MAY BE LOST IF DONE IMPROPERLY
#### PLEASE SEE THE ONLINE DOCUMENTATION FOR EXAMPLES

from swgpy.object import *

class Template(BaseTemplate):
	name = "object/installation/mining_liquid/base/shared_mining_liquid_base.iff"
	is_prototype = False
	
	def create(self, kernel, params):
		result = Installation()
	
		result.template = "object/installation/mining_liquid/base/shared_mining_liquid_base.iff"
		result.attribute_template_id = -1
		result.stfName("installation_name","moisture_harvester")		
		
		#### BEGIN MODIFICATIONS ####
		####  END MODIFICATIONS  ####
		
		return result

def loadTemplates(addTemplate):
	addTemplate(Template())