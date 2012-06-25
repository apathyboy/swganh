// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE

#ifndef SWGANH_TRE_IFF_READER_H_
#define SWGANH_TRE_IFF_READER_H_

#include <exception>
#include <list>
#include <memory>
#include <string>
#include <vector>

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

}}

#endif  // SWGANH_TRE_IFF_READER_H_
