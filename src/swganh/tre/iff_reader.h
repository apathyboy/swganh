// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_TRE_IFF_READER_H_
#define SWGANH_TRE_IFF_READER_H_

#include <exception>
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
        typedef std::runtime_error InvalidFormType;
    
        struct IffNode
        {
            IffNode();
            ~IffNode();
    
            size_t GetNodeSize() const;
            IffNode* FindNode(const std::string& node_name);    
            std::list<IffNode*> FindAllNodes(const std::string& node_name);
    
            std::string name;
            size_t size;
            std::vector<char> data;
            IffNode* parent;
            std::vector<std::unique_ptr<IffNode>> children;
        };
    
        IffReader(std::istream& input);
        ~IffReader();
    
        IffNode* FindNode(const std::string& node_name);
    
        std::list<IffNode*> FindAllNodes(const std::string& node_name);
    
    private:
        IffReader();

        struct IffHeader
        {
            char name[4];
            uint32_t size;
        };

        size_t ReadNodes_(std::istream& input, IffNode* parent);
        size_t GetNodeNameSize_(char* data);
        std::vector<std::unique_ptr<IffNode>> heads_;
    };
    
    typedef std::runtime_error BadFileFormat;

    class IffReaderV2
    {
    public:
        struct Node
        {
            char name[4];
            char type[4];
            uint32_t size;
            Node* parent;
            std::vector<char> data;
            std::vector<std::unique_ptr<Node>> children;
            
            Node* FindRecord(const std::string& record_name);
            std::list<Node*> FindAllRecords(const std::string& record_name);

            Node* FindForm(const std::string& form_name);
            std::list<Node*> FindAllForms(const std::string& form_name);
        };
        
        IffReaderV2(const std::vector<char>& input);

        Node* FindForm(const std::string& form_name);
        std::list<Node*> FindAllForms(const std::string& form_name);
    
    private:
        IffReaderV2();

        void ReadHead_(boost::archive::binary_iarchive& archive);
        void ReadNodes_(boost::archive::binary_iarchive& archive, Node* parent);

        std::unique_ptr<Node> head_;
    };

}}

#endif  // SWGANH_TRE_IFF_READER_H_
