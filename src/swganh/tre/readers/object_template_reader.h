// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_TRE_READERS_OBJECT_TEMPLATE_READER_H_
#define SWGANH_TRE_READERS_OBJECT_TEMPLATE_READER_H_

#include <memory>
#include <string>

#include "swganh/tre/iff_reader.h"

namespace anh {
namespace resource {
    class ResourceHandle;
}}

namespace swganh {
namespace object {
    class ObjectTemplate;
}}

namespace swganh {
namespace tre {
namespace readers {

    class ObjectTemplateReader
    {
    public:
        explicit ObjectTemplateReader(const std::shared_ptr<anh::resource::ResourceHandle>& resource);

        bool HasDerived() const;
        const std::string& GetDerivedType() const;
        const std::string& GetType() const;
        void ReadData(swganh::object::ObjectTemplate& output);

    private:
        void ReadType();
        void ReadDerived();

        IffReader iff_io_;
        std::string type_;
        std::string derived_name_;
        bool has_derived_;
    };

}}}

#endif  // SWGANH_TRE_READERS_OBJECT_TEMPLATE_READER_H_
