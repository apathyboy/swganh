// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_TRE_IFF_READER_H_
#define SWGANH_TRE_IFF_READER_H_

#include <stdexcept>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace boost {
namespace archive {
    class binary_iarchive;
}}

namespace swganh {
namespace tre {

    class IffReader
    {
    public:
        typedef std::runtime_error BadFileFormat;
        typedef std::invalid_argument InvalidFormType;
        typedef std::invalid_argument InvalidRecordType;

        struct Node
        {
            char name[4];
            char type[4];
            uint32_t size;
            Node* parent;
            std::vector<char> data;
            std::vector<std::unique_ptr<Node>> children;

            /**
             * Returns the first form found with the requested name.
             *
             * This method throws an exception when a requested name cannot be found making
             * it ideal for daisy-chaining requests:
             *
             * @code    node->Form("SCOT")->Form("STAT")->Form("SHOT")->Record("DERV");
             *
             * @throws InvalidFormType When no form by the requested name can be found.
             */
            Node* Form(const std::string& form_name);
            std::list<Node*> FindAllForms(const std::string& form_name);
            
            Node* Record(const std::string& record_name);
            std::list<Node*> FindAllRecords(const std::string& record_name);
        };
        
        IffReader(const std::vector<char>& input);

        Node* Form(const std::string& form_name);
        std::list<Node*> FindAllForms(const std::string& form_name);
        std::list<Node*> FindAllRecords(const std::string& record_name);
    
    private:
        IffReader();

        void ReadHead_(boost::archive::binary_iarchive& archive);
        void ReadNodes_(boost::archive::binary_iarchive& archive, Node* parent);

        std::unique_ptr<Node> head_;
    };

}}

#endif  // SWGANH_TRE_IFF_READER_H_
