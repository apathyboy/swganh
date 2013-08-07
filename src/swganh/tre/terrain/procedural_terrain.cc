
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
	{
		auto rgrp0003 = rgrp->form("0003");

		for (const auto& child : rgrp0003->children)
		{
			auto radial_fam = std::make_unique<radial_family>();
			radial_fam->load(child.get());

			save_list.push_back(radial_fam.get());
			radial_group.push_back(std::move(radial_fam));
		}
	}

	void load_environment_group(iff_node* egrp)
	{
		auto egrp0002 = egrp->form("0002");

		for (const auto& child : egrp0002->children)
		{
			auto environment_fam = std::make_unique<environment_family>();
			environment_fam->load(child->record("DATA"));

			save_list.push_back(environment_fam.get());
			environment_group.push_back(std::move(environment_fam));
		}
	}

	void load_fractal_group(iff_node* mgrp)
	{
		auto mgrp0000 = mgrp->form("0000");

		for (const auto& child : mgrp0000->children)
		{
			auto data = child->record("DATA");
			auto fractal = child->form("MFRC")->form("0001")->record("DATA");

			auto fractal_fam = std::make_unique<fractal_family>();
			fractal_fam->load(data);

			fractal_fam->fractal_data = std::make_unique<fractal_family::fractal>();
			fractal_fam->fractal_data->load(fractal);

			save_list.push_back(fractal_fam.get());
			save_list.push_back(fractal_fam->fractal_data.get());

			fractal_group.push_back(std::move(fractal_fam));
		}
	}

	void load_layers(iff_node* lyrs)
	{
		for (const auto& child : lyrs->children)
		{
			layers.push_back(load_layer(child.get()));
		}
	}
	
	std::unique_ptr<base_terrain_layer> load_layer(iff_node* layer_node, base_terrain_layer* parent = nullptr)
	{
		std::unique_ptr<base_terrain_layer> layer = nullptr;

		switch (layer_node->form_type)
		{
		case 0x5259414c:
			{
				layer = load_construction_layer(layer_node);
			}
			break;
		default:
			{
				layer = std::make_unique<default_layer>();

				if (layer_node->children[0]->children[1]->children.size() > 0) 
				{
					layer->load(layer_node->children[0]->children[1]->children[0].get());
				}
				else
				{
					layer->load(layer_node->children[0]->children[1].get());
				}

				layer->header = load_layer_header(layer_node->children[0]->children[0].get());

				save_list.push_back(layer.get());
			}
		}

		layer->parent = parent;

		return layer;
	}

	std::unique_ptr<layer_header> load_layer_header(iff_node* header_node)
	{
		auto header = std::make_unique<layer_header>();
		header->load(header_node->form("0001")->record("DATA"));
		save_list.push_back(header.get());
		return header;
	}

	std::unique_ptr<construction_layer> load_construction_layer(iff_node* node)
	{
		auto layr0003 = node->form("0003");

		auto layer = std::make_unique<construction_layer>();
		layer->header = load_layer_header(layr0003->form("IHDR"));
		layer->load(layr0003->record("ADTA"));

		save_list.push_back(layer.get());

		for (uint32_t i = 2; i < layr0003->children.size(); ++i)
		{
			auto child = load_layer(layr0003->children[i].get(), layer.get());
			layer->children.push_back(std::move(child));
		}

		return layer;
	}
	
	iff_node* iff_doc;
	std::vector<base_terrain_type*> save_list;
	
	header hdr;
	footer ftr;

	std::vector<std::unique_ptr<shader_family>> shader_group;
	std::vector<std::unique_ptr<flora_family>> flora_group;
	std::vector<std::unique_ptr<radial_family>> radial_group;
	std::vector<std::unique_ptr<environment_family>> environment_group;
	std::vector<std::unique_ptr<fractal_family>> fractal_group;
	std::vector<std::unique_ptr<base_terrain_layer>> layers;
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

