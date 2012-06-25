
#include "iff_reader.h"

#include <cstring>

#include "anh/utilities.h"

using swganh::tre::IffReader;

IffReader::IffNode::IffNode()
{}

IffReader::IffNode::~IffNode()
{}

IffReader::IffReader(std::istream& input)
{
    IffHeader header;
    input.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (std::strcmp(header.name, "FORM") != 0)
    {
        throw BadFileFormat("Invalid Iff file");
    }

    std::unique_ptr<IffNode> node(new IffNode);

    std::copy(std::begin(header.name), std::end(header.name), std::begin(node->name));
    node->size = anh::bigToHost(header.size);

    ReadNodes_(input, node.get());

    heads_.push_back(std::move(node));
}

IffReader::~IffReader()
{}

IffReader::IffNode* IffReader::FindNode(const std::string& node_name)
{
    return nullptr;
}

std::list<IffReader::IffNode*> IffReader::FindAllNodes(const std::string& node_name)
{
    std::list<IffReader::IffNode*> nodes;
    return nodes;
}

size_t IffReader::ReadNodes_(std::istream& input, IffNode* parent)
{
    size_t current_used = 0;

    while(current_used < parent->size)
    {
        size_t remaining = parent->size - current_used;

        char name_buffer[8];
        size_t name_size = 0;

        if (remaining >= 8)
        {
            input.read(name_buffer, 8);
            name_size = GetNodeNameSize_(name_buffer);
        }
        else
        {
            parent->data.resize(remaining);
            input.read(&parent->data[0], remaining);
            continue;
        }

        if (name_size == 0)
        {

        }
    }

    return 0;
}

size_t IffReader::GetNodeNameSize_(char* data)
{
	for(unsigned int i=0; i < 8; i++)
	{
		if(	!((data[i] >= 'A' && data[i] <= 'Z') || 
			  (data[i] >= '0' && data[i] <= '9') || data[i] == ' '))
		{
			if( i < 4)
				return 0;
			else
				return 4;
		}
	}

	return 8;
}
