
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
		auto tgen0000 = tgen->form("0000");

		load_shader_group(tgen0000->form("SGRP"));
		load_flora_group(tgen0000->form("FGRP"));
		load_radial_group(tgen0000->form("RGRP"));
		load_environment_group(tgen0000->form("EGRP"));
		load_fractal_group(tgen0000->form("MGRP"));
		load_layers(tgen0000->form("LYRS"));
	}

	void load_footer(iff_node* form0001)
	{
		ftr.load(form0001->record("DATA"));
		save_list.push_back(&ftr);
	}

	void load_shader_group(iff_node* sgrp)
	{
		auto sgrp0006 = sgrp->form("0006");

		for (const auto& child : sgrp0006->children)
		{
			auto shader_fam = std::make_unique<shader_family>();
			shader_fam->load(child.get());

			save_list.push_back(shader_fam.get());
			shader_group.push_back(std::move(shader_fam));
		}
	}

	void load_flora_group(iff_node* fgrp)
	{
		auto fgrp0008 = fgrp->form("0008");

		for (const auto& child : fgrp0008->children)
		{
			auto flora_fam = std::make_unique<flora_family>();
			flora_fam->load(child.get());

			save_list.push_back(flora_fam.get());
			flora_group.push_back(std::move(flora_fam));
		}
	}

	void load_radial_group(iff_node* rgrp)
	{}

	void load_environment_group(iff_node* egrp)
	{}

	void load_fractal_group(iff_node* mgrp)
	{}

	void load_layers(iff_node* lyrs)
	{}
	
	iff_node* iff_doc;
	std::vector<base_terrain_type*> save_list;
	
	header hdr;
	footer ftr;

	std::vector<std::unique_ptr<shader_family>> shader_group;
	std::vector<std::unique_ptr<flora_family>> flora_group;
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

