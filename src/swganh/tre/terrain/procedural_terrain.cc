
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
		load_layers(terrain.layers_, tgen0000->form("LYRS"));
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

	static void load_layers(std::vector<std::unique_ptr<base_terrain_layer>>& layers, iff_node* lyrs)
	{
		for (const auto& child : lyrs->children)
		{
			layers.push_back(load_layer(child.get()));
		}
	}

	static std::unique_ptr<base_terrain_layer> load_layer(iff_node* layer_node, base_terrain_layer* parent = nullptr)
	{
		std::unique_ptr<base_terrain_layer> layer = nullptr;
	
		switch (layer_node->form_type)
		{
		//case 0x5259414c: // LAYR
		//	{
		//		layer = load_construction_layer(layer_node);
		//	}
		//	break;
		case 0x4e434341: // ACCN
			{
				auto accn0000 = layer_node->form("0000");				
				layer = load_affector<affector_color_constant>(accn0000, accn0000->record("DATA"));
			}
			break;
		case 0x46524341: // ACRF
			{
				auto acrf0001 = layer_node->form("0001");
				layer = load_affector<affector_color_ramp_fractal>(acrf0001, acrf0001->form("DATA")->record("PARM"));
			}
			break;
		case 0x48524341: // ACRH
			{
				auto acrh0000 = layer_node->form("0000");
				layer = load_affector<affector_color_ramp_height>(acrh0000, acrh0000->record("DATA"));
			}
			break;
		case 0x564e4541: // AENV
			{
				auto aenv0000 = layer_node->form("0000");
				layer = load_affector<affector_environment>(aenv0000, aenv0000->record("DATA"));
			}
			break;
		case 0x43584541: // AEXC
			{
				auto aexc0000 = layer_node->form("0000");
				layer = load_affector<affector_exclude>(aexc0000, aexc0000->record("DATA"));
			}
			break;
		case 0x4e444641: // AFDN
			{
				auto afdn0002 = layer_node->form("0002");
				layer = load_affector<affector_radial_near_constant>(afdn0002, afdn0002->record("DATA"));
			}
			break;
		case 0x46444641: // AFDF
			{
				auto afdf0002 = layer_node->form("0002");
				layer = load_affector<affector_radial_far_constant>(afdf0002, afdf0002->record("DATA"));
			}
			break;
		case 0x43534641: // AFSC
			{
				auto afsc0004 = layer_node->form("0004");
				layer = load_affector<affector_flora_collidable_constant>(afsc0004, afsc0004->record("DATA"));
			}
			break;
		case 0x4e534641: // AFSN
			{
				auto afsn0004 = layer_node->form("0004");
				layer = load_affector<affector_flora_non_collidable_constant>(afsn0004, afsn0004->record("DATA"));
			}
			break;
		case 0x4e434841: // AHCN
			{
				auto ahcn0000 = layer_node->form("0000");
				layer = load_affector<affector_height_constant>(ahcn0000, ahcn0000->record("DATA"));
			}
			break;
		case 0x52464841: // AHFR
			{
				auto ahfr0003 = layer_node->form("0003");
				layer = load_affector<affector_height_fractal>(ahfr0003, ahfr0003->form("DATA")->record("PARM"));
			}
			break;
		case 0x52544841: // AHTR
			{
				auto ahtr0004 = layer_node->form("0004");
				layer = load_affector<affector_height_terrace>(ahtr0004, ahtr0004->record("DATA"));
			}
			break;
		//case 0x56495241: // ARIV
		//	{
		//		layer = load_affector_river(layer_node);
		//	}
		//	break;
		//case 0x414f5241: // AROA
		//	{
		//		layer = load_affector_road(layer_node);
		//	}
		//	break;
		case 0x4e435341: // ASCN
			{
				auto ascn0001 = layer_node->form("0001");
				layer = load_affector<affector_shader_constant>(ascn0001, ascn0001->record("DATA"));
			}
			break;
		case 0x50525341: // ASRP
			{
				auto asrp0001 = layer_node->form("0001");
				layer = load_affector<affector_shader_replace>(asrp0001, asrp0001->record("DATA"));
			}
			break;
		case 0x52494342: // BCIR
			{
				auto bcir0002 = layer_node->form("0002");
				layer = load_affector<boundary_circle>(bcir0002, bcir0002->record("DATA"));
			}
			break;
		case 0x4e4c5042: // BPLN
			{
				auto bpln0001 = layer_node->form("0001");
				layer = load_affector<boundary_polyline>(bpln0001, bpln0001->record("DATA"));
			}
			break;
		case 0x4c4f5042: // BPOL
			{
				auto bpol0005 = layer_node->form("0005");
				layer = load_affector<boundary_polygon>(bpol0005, bpol0005->record("DATA"));
			}
			break;
		case 0x43455242: // BREC
			{
				auto brec0003 = layer_node->form("0003");
				layer = load_affector<boundary_rectangle>(brec0003, brec0003->record("DATA"));
			}
			break;
		case 0x52494446: // FDIR
			{
				auto fdir0000 = layer_node->form("0000");
				layer = load_affector<filter_direction>(fdir0000, fdir0000->record("DATA"));
			}
			break;
		case 0x41524646: // FFRA
			{
				auto ffra0005 = layer_node->form("0005");
				layer = load_affector<filter_fractal>(ffra0005, ffra0005->form("DATA")->record("PARM"));
			}
			break;
		case 0x54474846: // FHGT
			{
				auto fhgt0002 = layer_node->form("0002");
				layer = load_affector<filter_height>(fhgt0002, fhgt0002->record("DATA"));
			}
			break;
		case 0x44485346: // FSHD
			{
				auto fshd0000 = layer_node->form("0000");
				layer = load_affector<filter_shader>(fshd0000, fshd0000->record("DATA"));
			}
			break;
		case 0x504c5346: // FSLP
			{
				auto fslp0002 = layer_node->form("0002");
				layer = load_affector<filter_slope>(fslp0002, fslp0002->record("DATA"));
			}
			break;
		default:
			{
				throw std::runtime_error("Unknown layer type: " + layer_node->str_form_type());
			}
		}
	
		if (layer)
		{
			layer->parent = parent;
		}

		return layer;
	}

	//template<typename T>
	//static void create_affector()

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

	template<typename T>
	static std::unique_ptr<T> load_affector(iff_node* layer_node, iff_node* data_node)
	{
		auto layer = std::make_unique<T>();
		static_cast<base_terrain_layer*>(layer.get())->deserialize(layer_node->form("IHDR")->form("0001")->record("DATA")->data);
		layer->deserialize(data_node->data);

		return layer;
	}
	
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
