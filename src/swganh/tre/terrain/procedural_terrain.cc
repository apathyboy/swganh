
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

	template<typename T>
	std::unique_ptr<T> make_node_data(iff_node* node)
	{
		auto node_data = std::make_unique<T>();
		node_data->load(node);

		save_list.push_back(node_data.get());

		return node_data;
	}

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
			shader_group.push_back(make_node_data<shader_family>(child.get()));
		}
	}

	void load_flora_group(iff_node* fgrp)
	{
		auto fgrp0008 = fgrp->form("0008");

		for (const auto& child : fgrp0008->children)
		{
			flora_group.push_back(make_node_data<flora_family>(child.get()));
		}
	}

	void load_radial_group(iff_node* rgrp)
	{
		auto rgrp0003 = rgrp->form("0003");

		for (const auto& child : rgrp0003->children)
		{
			radial_group.push_back(make_node_data<radial_family>(child.get()));
		}
	}

	void load_environment_group(iff_node* egrp)
	{
		auto egrp0002 = egrp->form("0002");

		for (const auto& child : egrp0002->children)
		{
			environment_group.push_back(make_node_data<environment_family>(child->record("DATA")));
		}
	}

	void load_fractal_group(iff_node* mgrp)
	{
		auto mgrp0000 = mgrp->form("0000");

		for (const auto& child : mgrp0000->children)
		{
			auto data = child->record("DATA");
			auto fractal = child->form("MFRC")->form("0001")->record("DATA");

			auto fractal_fam = make_node_data<fractal_family>(data);
			fractal_fam->fractal_data = make_node_data<fractal_family::fractal>(fractal);

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
		case 0x5259414c: // LAYR
			{
				layer = load_construction_layer(layer_node);
			}
			break;
		case 0x46524341: // ACRF
			{
				layer = load_affector_color_ramp_fractal(layer_node);
			}
			break;
		case 0x564e4541: // AENV
			{
				layer = load_affector_environment(layer_node);
			}
			break;
		case 0x43584541: // AEXC
			{
				layer = load_affector_exclude(layer_node);
			}
			break;
		case 0x4e444641: // AFDN
			{
				layer = load_affector_radial_near_constant(layer_node);
			}
			break;
		case 0x4e534641: // AFSN
			{
				layer = load_affector_flora_non_collidable_constant(layer_node);
			}
			break;
		case 0x52464841: // AHFR
			{
				layer = load_affector_height_fractal(layer_node);
			}
			break;
		case 0x4e435341: // ASCN
			{
				layer = load_affector_shader_constant(layer_node);
			}
			break;
		case 0x4c4f5042: // BPOL
			{
				layer = load_boundary_polygon(layer_node);
			}
			break;
		case 0x43455242: // BREC
			{
				layer = load_boundary_rectangle(layer_node);
			}
			break;
		case 0x44485346: // FSHD
			{
				layer = load_filter_shader(layer_node);
			}
			break;
		case 0x504c5346: // FSLP
			{
				layer = load_filter_slope(layer_node);
			}
			break;
		default:
			{
				if (layer_node->children[0]->children[1]->children.size() > 0) 
				{
					layer = make_node_data<default_layer>(layer_node->children[0]->children[1]->children[0].get());
				}
				else
				{
					layer = make_node_data<default_layer>(layer_node->children[0]->children[1].get());
				}

				layer->header = load_layer_header(layer_node->children[0]->children[0].get());
			}
		}

		layer->parent = parent;

		return layer;
	}

	std::unique_ptr<layer_header> load_layer_header(iff_node* header_node)
	{
		return make_node_data<layer_header>(header_node->form("0001")->record("DATA"));
	}

	std::unique_ptr<construction_layer> load_construction_layer(iff_node* node)
	{
		auto layr0003 = node->form("0003");

		auto layer = make_node_data<construction_layer>(layr0003->record("ADTA"));
		layer->header = load_layer_header(layr0003->form("IHDR"));
		
		for (uint32_t i = 2; i < layr0003->children.size(); ++i)
		{
			auto child = load_layer(layr0003->children[i].get(), layer.get());
			layer->children.push_back(std::move(child));
		}

		return layer;
	}
	
	std::unique_ptr<affector_color_ramp_fractal> load_affector_color_ramp_fractal(iff_node* acrf)
	{
		auto acrf0001 = acrf->form("0001");

		auto layer = make_node_data<affector_color_ramp_fractal>(acrf0001->form("DATA")->record("PARM"));
		layer->header = load_layer_header(acrf0001->form("IHDR"));

		return layer;
	}

	std::unique_ptr<affector_environment> load_affector_environment(iff_node* aenv)
	{
		auto aenv0000 = aenv->form("0000");

		auto layer = make_node_data<affector_environment>(aenv0000->record("DATA"));
		layer->header = load_layer_header(aenv0000->form("IHDR"));

		return layer;
	}

	std::unique_ptr<affector_exclude> load_affector_exclude(iff_node* aexc)
	{
		auto aexc0000 = aexc->form("0000");

		auto layer = make_node_data<affector_exclude>(aexc0000->record("DATA"));
		layer->header = load_layer_header(aexc0000->form("IHDR"));

		return layer;
	}

	std::unique_ptr<affector_flora_non_collidable_constant> load_affector_flora_non_collidable_constant(iff_node* afsn)
	{
		auto afsn0004 = afsn->form("0004");

		auto layer = make_node_data<affector_flora_non_collidable_constant>(afsn0004->record("DATA"));
		layer->header = load_layer_header(afsn0004->form("IHDR"));

		return layer;
	}

	std::unique_ptr<affector_height_fractal> load_affector_height_fractal(iff_node* ahfr)
	{
		auto ahfr0003 = ahfr->form("0003");

		auto layer = make_node_data<affector_height_fractal>(ahfr0003->form("DATA")->record("PARM"));
		layer->header = load_layer_header(ahfr0003->form("IHDR"));

		return layer;
	}

	std::unique_ptr<affector_radial_near_constant> load_affector_radial_near_constant(iff_node* afdn)
	{
		auto afdn0002 = afdn->form("0002");

		auto layer = make_node_data<affector_radial_near_constant>(afdn0002->record("DATA"));
		layer->header = load_layer_header(afdn0002->form("IHDR"));

		return layer;
	}

	std::unique_ptr<boundary_polygon> load_boundary_polygon(iff_node* bpol)
	{
		auto bpol0005 = bpol->form("0005");

		auto layer = make_node_data<boundary_polygon>(bpol0005->record("DATA"));
		layer->header = load_layer_header(bpol0005->form("IHDR"));

		return layer;
	}

	std::unique_ptr<boundary_rectangle> load_boundary_rectangle(iff_node* brec)
	{
		auto brec0003 = brec->form("0003");

		auto layer = make_node_data<boundary_rectangle>(brec0003->record("DATA"));
		layer->header = load_layer_header(brec0003->form("IHDR"));

		return layer;
	}

	std::unique_ptr<affector_shader_constant> load_affector_shader_constant(iff_node* ascn)
	{
		auto ascn0001 = ascn->form("0001");

		auto layer = make_node_data<affector_shader_constant>(ascn0001->record("DATA"));
		layer->header = load_layer_header(ascn0001->form("IHDR"));

		return layer;
	}

	std::unique_ptr<filter_shader> load_filter_shader(iff_node* fshd)
	{
		auto fshd0000 = fshd->form("0000");

		auto layer = make_node_data<filter_shader>(fshd0000->record("DATA"));
		layer->header = load_layer_header(fshd0000->form("IHDR"));

		return layer;
	}

	std::unique_ptr<filter_slope> load_filter_slope(iff_node* fslp)
	{
		auto fslp0002 = fslp->form("0002");

		auto layer = make_node_data<filter_slope>(fslp0002->record("DATA"));
		layer->header = load_layer_header(fslp0002->form("IHDR"));

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

