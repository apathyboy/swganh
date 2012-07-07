
#ifndef SWGANH_OBJECT_SHARED_OBJECT_TEMPLATE_H_
#define SWGANH_OBJECT_SHARED_OBJECT_TEMPLATE_H_

#include "object_template.h"
#include "anh/property/property_types.h"

namespace swganh {
namespace object {

    class SharedObjectTemplate : public ObjectTemplate
    {
    public:
        explicit SharedObjectTemplate(std::string name, ObjectTemplate* derived = nullptr);
        ~SharedObjectTemplate();

    private:
        void BuildPropertyMap();
        
        anh::property::StringList object_name_;
        anh::property::StringList detailedDescription;
        anh::property::StringList lookAtText;
        bool snapToTerrain;
        uint32_t containerType;
        uint32_t containerVolumeLimit;
        std::string tintPalette;
        std::string slotDescriptorFilename;
        std::string arrangementDescriptorFilename;
        std::string appearanceFilename;
        std::string portalLayoutFilename;
        std::string clientDataFile;
        uint32_t collisionMaterialFlags;
        uint32_t collisionMaterialPassFlags;
        uint32_t collisionMaterialBlockFlags;
        uint32_t collisionActionFlags;
        uint32_t collisionActionPassFlags;
        uint32_t collisionActionBlockFlags;
        float scale;
        uint32_t gameObjectType;
        bool sendToClient;
        float scaleThresholdBeforeExtentTest;
        float clearFloraRadius;
        uint32_t surfaceType;
        float noBuildRadius;
        bool onlyVisibleInTools;
        float locationReservationRadius;
    };

}}

#endif  // SWGANH_OBJECT_SHARED_OBJECT_TEMPLATE_H_
