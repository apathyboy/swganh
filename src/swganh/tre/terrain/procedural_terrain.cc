
#include "procedural_terrain.h"

#include <vector>

using namespace swganh::tre::detail_terrain;
using swganh::ByteBuffer;
using swganh::tre::iff_node;
using swganh::tre::procedural_terrain;
using swganh::tre::procedural_terrain_impl;

struct terrain_iff_helper
{
	static void load_header(procedural_terrain& terrain, iff_node* data)
	{
		terrain.header.deserialize(data->data);
	}

	static void load_terrain_data(procedural_terrain& terrain, iff_node* tgen)
	{
		auto tgen0000 = tgen->form("0000");
	
		load_group(terrain.shader_group_, tgen0000->form("SGRP")->form("0006"));
		load_group(terrain.flora_group_, tgen0000->form("FGRP")->form("0008"));
		load_group(terrain.radial_group_, tgen0000->form("RGRP")->form("0003"));
		load_group(terrain.environment_group_, tgen0000->form("EGRP")->form("0002"));
		load_group(terrain.fractal_group_, tgen0000->form("MGRP")->form("0000"));
		//load_layers(tgen0000->form("LYRS"));
	}
	
	static void load_footer(procedural_terrain& terrain, iff_node* form0001)
	{
		auto footer_data = form0001->record("DATA");
	
		terrain.footer.deserialize(footer_data->data);
	}


	template<typename T>
	static void load_group(terrain_group<T>& group, iff_node* group_node)
	{
		for (const auto& child : group_node->children)
		{
			auto family = std::make_unique<T>();
			family->deserialize(child->data);

			group.add_family(std::move(family));
		}
	}

	template<>
	static void load_group(terrain_group<fractal_family>& group, iff_node* group_node)
	{
		for (const auto& child : group_node->children)
		{
			auto data = child->record("DATA");
			auto fractal = child->form("MFRC")->form("0001")->record("DATA");
	
			auto fractal_fam = std::make_unique<fractal_family>();
			fractal_fam->deserialize(data->data);
			fractal_fam->fractal_data = std::make_unique<fractal_family::fractal>();
			fractal_fam->fractal_data->deserialize(fractal->data);
	
			group.add_family(std::move(fractal_fam));
		}	
	}

	//void load_layers(iff_node* lyrs)
	//{
	//	for (const auto& child : lyrs->children)
	//	{
	//		layers.push_back(load_layer(child.get()));
	//	}
	//}

	//std::unique_ptr<base_terrain_layer> load_layer(iff_node* layer_node, base_terrain_layer* parent = nullptr)
	//{
	//	std::unique_ptr<base_terrain_layer> layer = nullptr;
	//
	//	switch (layer_node->form_type)
	//	{
	//	case 0x5259414c: // LAYR
	//		{
	//			layer = load_construction_layer(layer_node);
	//		}
	//		break;
	//	case 0x4e434341: // ACCN
	//		{
	//			layer = load_affector_color_constant(layer_node);
	//		}
	//		break;
	//	case 0x46524341: // ACRF
	//		{
	//			layer = load_affector_color_ramp_fractal(layer_node);
	//		}
	//		break;
	//	case 0x48524341: // ACRH
	//		{
	//			layer = load_affector_color_ramp_height(layer_node);
	//		}
	//		break;
	//	case 0x564e4541: // AENV
	//		{
	//			layer = load_affector_environment(layer_node);
	//		}
	//		break;
	//	case 0x43584541: // AEXC
	//		{
	//			layer = load_affector_exclude(layer_node);
	//		}
	//		break;
	//	case 0x4e444641: // AFDN
	//		{
	//			layer = load_affector_radial_near_constant(layer_node);
	//		}
	//		break;
	//	case 0x46444641: // AFDF
	//		{
	//			layer = load_affector_radial_far_constant(layer_node);
	//		}
	//		break;
	//	case 0x43534641: // AFSC
	//		{
	//			layer = load_affector_flora_collidable_constant(layer_node);
	//		}
	//		break;
	//	case 0x4e534641: // AFSN
	//		{
	//			layer = load_affector_flora_non_collidable_constant(layer_node);
	//		}
	//		break;
	//	case 0x4e434841: // AHCN
	//		{
	//			layer = load_affector_height_constant(layer_node);
	//		}
	//		break;
	//	case 0x52464841: // AHFR
	//		{
	//			layer = load_affector_height_fractal(layer_node);
	//		}
	//		break;
	//	case 0x52544841: // AHTR
	//		{
	//			layer = load_affector_height_terrace(layer_node);
	//		}
	//		break;
	//	case 0x56495241: // ARIV
	//		{
	//			layer = load_affector_river(layer_node);
	//		}
	//		break;
	//	case 0x414f5241: // AROA
	//		{
	//			layer = load_affector_road(layer_node);
	//		}
	//		break;
	//	case 0x4e435341: // ASCN
	//		{
	//			layer = load_affector_shader_constant(layer_node);
	//		}
	//		break;
	//	case 0x50525341: // ASRP
	//		{
	//			layer = load_affector_shader_replace(layer_node);
	//		}
	//		break;
	//	case 0x52494342: // BCIR
	//		{
	//			layer = load_boundary_circle(layer_node);
	//		}
	//		break;
	//	case 0x4e4c5042: // BPLN
	//		{
	//			layer = load_boundary_polyline(layer_node);
	//		}
	//		break;
	//	case 0x4c4f5042: // BPOL
	//		{
	//			layer = load_boundary_polygon(layer_node);
	//		}
	//		break;
	//	case 0x43455242: // BREC
	//		{
	//			layer = load_boundary_rectangle(layer_node);
	//		}
	//		break;
	//	case 0x52494446: // FDIR
	//		{
	//			layer = load_filter_direction(layer_node);
	//		}
	//		break;
	//	case 0x41524646: // FFRA
	//		{
	//			layer = load_filter_fractal(layer_node);
	//		}
	//		break;
	//	case 0x54474846: // FHGT
	//		{
	//			layer = load_filter_height(layer_node);
	//		}
	//		break;
	//	case 0x44485346: // FSHD
	//		{
	//			layer = load_filter_shader(layer_node);
	//		}
	//		break;
	//	case 0x504c5346: // FSLP
	//		{
	//			layer = load_filter_slope(layer_node);
	//		}
	//		break;
	//	default:
	//		{
	//			throw std::runtime_error("Unknown layer type: " + layer_node->str_form_type());
	//		}
	//	}
	//
	//	layer->parent = parent;
	//
	//	return layer;
	//}

	//std::unique_ptr<layer_header> load_layer_header(iff_node* header_node)
	//{
	//	return make_node_data<layer_header>(header_node->form("0001")->record("DATA"));
	//}

	//std::unique_ptr<construction_layer> load_construction_layer(iff_node* node)
	//{
	//	auto layr0003 = node->form("0003");
	//
	//	auto layer = make_node_data<construction_layer>(layr0003->record("ADTA"));
	//	layer->header = load_layer_header(layr0003->form("IHDR"));
	//	
	//	for (uint32_t i = 2; i < layr0003->children.size(); ++i)
	//	{
	//		auto child = load_layer(layr0003->children[i].get(), layer.get());
	//		layer->children.push_back(std::move(child));
	//	}
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_color_constant> load_affector_color_constant(iff_node* accn)
	//{
	//	auto accn0000 = accn->form("0000");
	//
	//	auto layer = make_node_data<affector_color_constant>(accn0000->record("DATA"));
	//	layer->header = load_layer_header(accn0000->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_color_ramp_fractal> load_affector_color_ramp_fractal(iff_node* acrf)
	//{
	//	auto acrf0001 = acrf->form("0001");
	//
	//	auto layer = make_node_data<affector_color_ramp_fractal>(acrf0001->form("DATA")->record("PARM"));
	//	layer->header = load_layer_header(acrf0001->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_color_ramp_height> load_affector_color_ramp_height(iff_node* acrh)
	//{
	//	auto acrh0000 = acrh->form("0000");
	//
	//	auto layer = make_node_data<affector_color_ramp_height>(acrh0000->record("DATA"));
	//	layer->header = load_layer_header(acrh0000->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_environment> load_affector_environment(iff_node* aenv)
	//{
	//	auto aenv0000 = aenv->form("0000");
	//
	//	auto layer = make_node_data<affector_environment>(aenv0000->record("DATA"));
	//	layer->header = load_layer_header(aenv0000->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_exclude> load_affector_exclude(iff_node* aexc)
	//{
	//	auto aexc0000 = aexc->form("0000");
	//
	//	auto layer = make_node_data<affector_exclude>(aexc0000->record("DATA"));
	//	layer->header = load_layer_header(aexc0000->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_flora_collidable_constant> load_affector_flora_collidable_constant(iff_node* afsc)
	//{
	//	auto afsc0004 = afsc->form("0004");
	//
	//	auto layer = make_node_data<affector_flora_collidable_constant>(afsc0004->record("DATA"));
	//	layer->header = load_layer_header(afsc0004->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_flora_non_collidable_constant> load_affector_flora_non_collidable_constant(iff_node* afsn)
	//{
	//	auto afsn0004 = afsn->form("0004");
	//
	//	auto layer = make_node_data<affector_flora_non_collidable_constant>(afsn0004->record("DATA"));
	//	layer->header = load_layer_header(afsn0004->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_height_constant> load_affector_height_constant(iff_node* ahcn)
	//{
	//	auto ahcn0000 = ahcn->form("0000");
	//
	//	auto layer = make_node_data<affector_height_constant>(ahcn0000->record("DATA"));
	//	layer->header = load_layer_header(ahcn0000->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_height_fractal> load_affector_height_fractal(iff_node* ahfr)
	//{
	//	auto ahfr0003 = ahfr->form("0003");
	//
	//	auto layer = make_node_data<affector_height_fractal>(ahfr0003->form("DATA")->record("PARM"));
	//	layer->header = load_layer_header(ahfr0003->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_height_terrace> load_affector_height_terrace(iff_node* ahtr)
	//{
	//	auto ahtr0004 = ahtr->form("0004");
	//
	//	auto layer = make_node_data<affector_height_terrace>(ahtr0004->record("DATA"));
	//	layer->header = load_layer_header(ahtr0004->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_radial_far_constant> load_affector_radial_far_constant(iff_node* afdf)
	//{
	//	auto afdf0002 = afdf->form("0002");
	//
	//	auto layer = make_node_data<affector_radial_far_constant>(afdf0002->record("DATA"));
	//	layer->header = load_layer_header(afdf0002->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_radial_near_constant> load_affector_radial_near_constant(iff_node* afdn)
	//{
	//	auto afdn0002 = afdn->form("0002");
	//
	//	auto layer = make_node_data<affector_radial_near_constant>(afdn0002->record("DATA"));
	//	layer->header = load_layer_header(afdn0002->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_river> load_affector_river(iff_node* ariv)
	//{
	//	auto ariv0005 = ariv->form("0005");
	//
	//	auto layer = make_node_data<affector_river>(ariv0005->form("DATA")->record("DATA"));
	//	layer->header = load_layer_header(ariv0005->form("IHDR"));
	//
	//	for (const auto& child : ariv0005->form("DATA")->form("HDTA")->form("0001")->children)
	//	{
	//		auto segment = make_node_data<affector_river::river_segment>(child.get());
	//		layer->segments.push_back(std::move(segment));
	//	}
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_road> load_affector_road(iff_node* aroa)
	//{
	//	auto aroa0005 = aroa->form("0005");
	//
	//	auto layer = make_node_data<affector_road>(aroa0005->form("DATA")->record("DATA"));
	//	layer->header = load_layer_header(aroa0005->form("IHDR"));
	//
	//	for (const auto& child : aroa0005->form("DATA")->form("HDTA")->form("0001")->children)
	//	{
	//		auto segment = make_node_data<affector_road::road_segment>(child.get());
	//		layer->segments.push_back(std::move(segment));
	//	}
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_shader_constant> load_affector_shader_constant(iff_node* ascn)
	//{
	//	auto ascn0001 = ascn->form("0001");
	//
	//	auto layer = make_node_data<affector_shader_constant>(ascn0001->record("DATA"));
	//	layer->header = load_layer_header(ascn0001->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<affector_shader_replace> load_affector_shader_replace(iff_node* ascn)
	//{
	//	auto ascn0001 = ascn->form("0001");
	//
	//	auto layer = make_node_data<affector_shader_replace>(ascn0001->record("DATA"));
	//	layer->header = load_layer_header(ascn0001->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<boundary_circle> load_boundary_circle(iff_node* bcir)
	//{
	//	auto bcir0002 = bcir->form("0002");
	//
	//	auto layer = make_node_data<boundary_circle>(bcir0002->record("DATA"));
	//	layer->header = load_layer_header(bcir0002->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<boundary_polygon> load_boundary_polygon(iff_node* bpol)
	//{
	//	auto bpol0005 = bpol->form("0005");
	//
	//	auto layer = make_node_data<boundary_polygon>(bpol0005->record("DATA"));
	//	layer->header = load_layer_header(bpol0005->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<boundary_polyline> load_boundary_polyline(iff_node* bpln)
	//{
	//	auto bpln0001 = bpln->form("0001");
	//
	//	auto layer = make_node_data<boundary_polyline>(bpln0001->record("DATA"));
	//	layer->header = load_layer_header(bpln0001->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<boundary_rectangle> load_boundary_rectangle(iff_node* brec)
	//{
	//	auto brec0003 = brec->form("0003");
	//
	//	auto layer = make_node_data<boundary_rectangle>(brec0003->record("DATA"));
	//	layer->header = load_layer_header(brec0003->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<filter_direction> load_filter_direction(iff_node* fdir)
	//{
	//	auto fdir0000 = fdir->form("0000");
	//
	//	auto layer = make_node_data<filter_direction>(fdir0000->record("DATA"));
	//	layer->header = load_layer_header(fdir0000->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<filter_fractal> load_filter_fractal(iff_node* ffra)
	//{
	//	auto ffra0005 = ffra->form("0005");
	//
	//	auto layer = make_node_data<filter_fractal>(ffra0005->form("DATA")->record("PARM"));
	//	layer->header = load_layer_header(ffra0005->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<filter_height> load_filter_height(iff_node* fhgt)
	//{
	//	auto fhgt0002 = fhgt->form("0002");
	//
	//	auto layer = make_node_data<filter_height>(fhgt0002->record("DATA"));
	//	layer->header = load_layer_header(fhgt0002->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<filter_shader> load_filter_shader(iff_node* fshd)
	//{
	//	auto fshd0000 = fshd->form("0000");
	//
	//	auto layer = make_node_data<filter_shader>(fshd0000->record("DATA"));
	//	layer->header = load_layer_header(fshd0000->form("IHDR"));
	//
	//	return layer;
	//}

	//std::unique_ptr<filter_slope> load_filter_slope(iff_node* fslp)
	//{
	//	auto fslp0002 = fslp->form("0002");
	//
	//	auto layer = make_node_data<filter_slope>(fslp0002->record("DATA"));
	//	layer->header = load_layer_header(fslp0002->form("IHDR"));
	//
	//	return layer;
	//}
};

//
//procedural_terrain::procedural_terrain()
//	: impl_(new procedural_terrain_impl)
//{}
//
//procedural_terrain::procedural_terrain(ByteBuffer& buffer)
//	: impl_(new procedural_terrain_impl())
//{
//	impl_->iff_doc = parse_iff(buffer);
//	load();
//}
//
//procedural_terrain::~procedural_terrain()
//{
//	save();
//}
//
//void procedural_terrain::load()
//{
//	auto form0014 = impl_->iff_doc->form("0014");
//
//	if (form0014)
//	{
//		impl_->load_header(form0014->record("DATA"));
//		impl_->load_terrain_data(form0014->form("TGEN"));
//		impl_->load_footer(form0014->form("0001"));
//	}
//}
//
//void procedural_terrain::save()
//{
//	for (const auto& to_save : impl_->save_list)
//	{
//		to_save.first->data.clear();
//		to_save.second->serialize(to_save.first->data);
//	}
//}
//
//iff_node* procedural_terrain::iff_doc() const
//{
//	return impl_->iff_doc.get();
//}
//
//terrain_group<shader_family>& procedural_terrain::shader_group()
//{
//	return impl_->shader_group;
//}
//
//terrain_group<flora_family>& procedural_terrain::flora_group()
//{
//	return impl_->flora_group;
//}
//
//terrain_group<radial_family>& procedural_terrain::radial_group()
//{
//	return impl_->radial_group;
//}
//
//terrain_group<environment_family>& procedural_terrain::environment_group()
//{
//	return impl_->environment_group;
//}
//
//terrain_group<fractal_family>& procedural_terrain::fractal_group()
//{
//	return impl_->fractal_group;
//}
//
//std::vector<std::unique_ptr<base_terrain_layer>>& procedural_terrain::layers()
//{
//	return impl_->layers;
//}

std::unique_ptr<procedural_terrain> swganh::tre::read_procedural_terrain(ByteBuffer& buffer)
{
	auto iff_doc = parse_iff(buffer);

	std::unique_ptr<procedural_terrain> pt;

	auto form0014 = iff_doc->form("0014");

	if (form0014)
	{
		terrain_iff_helper::load_header(*pt, form0014->record("DATA"));
		terrain_iff_helper::load_terrain_data(*pt, form0014->form("TGEN"));
		terrain_iff_helper::load_footer(*pt, form0014->form("0001"));
	}

	return pt;
}

ByteBuffer swganh::tre::write_procedural_terrain(procedural_terrain& terrain)
{
	return ByteBuffer();
}
