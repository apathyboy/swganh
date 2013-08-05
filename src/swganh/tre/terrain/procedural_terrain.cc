
#include "procedural_terrain.h"

#include <vector>

using namespace swganh::tre::detail_terrain;
using swganh::tre::iff_node;
using swganh::tre::procedural_terrain;
using swganh::tre::procedural_terrain_impl;

struct procedural_terrain_impl
{
	procedural_terrain_impl(iff_node* head)
		: iff_doc(head) {}

	void load_header(iff_node* data)
	{
		hdr.load(data);
		save_list.push_back(&hdr);
	}

	void load_terrain_data(iff_node* tgen)
	{

	}

	void load_footer(iff_node* form0001)
	{
		ftr.load(form0001->record("DATA"));
		save_list.push_back(&ftr);
	}

	iff_node* iff_doc;
	std::vector<base_terrain_type*> save_list;
	
	header hdr;
	footer ftr;
};

procedural_terrain::procedural_terrain(swganh::tre::iff_node* head)
	: impl_(new procedural_terrain_impl(head))
{}

procedural_terrain::~procedural_terrain()
{}

void procedural_terrain::load()
{
	auto form00014 = impl_->iff_doc->form("0014");

	impl_->load_header(form00014->record("DATA"));
	impl_->load_terrain_data(form00014->form("TGEN"));
	impl_->load_footer(form00014->form("0001"));
}

void procedural_terrain::save()
{
	for (auto to_save : impl_->save_list)
	{
		to_save->save();
	}
}

const iff_node& procedural_terrain::iff_doc() const
{
	return *impl_->iff_doc;
}

void procedural_terrain::set_filename(std::string filename)
{
	impl_->hdr.filename = filename;
}

std::string procedural_terrain::get_filename() const
{
	return impl_->hdr.filename;
}

