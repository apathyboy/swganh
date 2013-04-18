import re
from swgpy.command import BaseSwgCommand
from swgpy.utility import *



class TransferItemArmor(BaseSwgCommand):
    def getCommandName(self):
        return 'transferitemarmor'

    def run(self):
        split = re.split('\W+', self.getCommandString())
        container_id = split[1]

        container = self.getKernel().serviceManager().simulationService().findObjectById(int(container_id))

        if self.getActor() is container:
            container.remove(self.getActor(), self.getTarget())
            self.getActor().addToSlot(self.getTarget())
        else:
            print('Unequipped item start armor')
            self.getActor().clearFromSlot(self.getTarget())
            #print('Unequipped item end')
            #container.add(self.getActor(), self.getTarget())

