
#include "procedural_terrain.h"

#include <vector>

#include "swganh/tre/iff/iff.h"

using namespace swganh::tre::detail_terrain;
using swganh::ByteBuffer;
using swganh::tre::iff_node;
using swganh::tre::procedural_terrain;

struct terrain_iff_reader
{
	static void load_header(procedural_terrain& terrain, iff_node* data)
	{
		terrain.header.deserialize(data->data);
	}

	static void load_terrain_data(procedural_terrain& terrain, iff_node* tgen)
	{
		auto tgen0000 = tgen->form("0000");

		load_group(terrain.shader_group, tgen0000->form("SGRP")->form("0006"));
		load_group(terrain.flora_group, tgen0000->form("FGRP")->form("0008"));
		load_group(terrain.radial_group, tgen0000->form("RGRP")->form("0003"));
		load_group(terrain.environment_group, tgen0000->form("EGRP")->form("0002"));
		load_group(terrain.fractal_group, tgen0000->form("MGRP")->form("0000"));
		load_layers(terrain.layers, tgen0000->form("LYRS"));
	}

	static void load_footer(procedural_terrain& terrain, iff_node* form0001)
	{
		terrain.footer.deserialize(form0001->record("DATA")->data);
		terrain.water_map.deserialize(form0001->record("WMAP")->data);
		terrain.slope_map.deserialize(form0001->record("SMAP")->data);
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
	static void load_group(terrain_group<environment_family>& group, iff_node* group_node)
	{
		for (const auto& child : group_node->children)
		{
			auto family = std::make_unique<environment_family>();
			family->deserialize(child->record("DATA")->data);

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

	static void load_layers(std::vector < std::unique_ptr < construction_layer >> &layers, iff_node* lyrs)
	{
		for (const auto& child : lyrs->children)
		{
			layers.push_back(load_construction_layer(child.get()));
		}
	}

	static void load_layer(iff_node* layer_node, construction_layer* parent = nullptr)
	{
		switch (layer_node->form_type)
		{
		case 0x5259414c: // LAYR
			{
				load_construction_layer(layer_node, parent);
			}
			break;
		case 0x4e434341: // ACCN
			{
				auto accn0000 = layer_node->form("0000");
				load_affector<affector_color_constant>(accn0000, accn0000->record("DATA"), parent);
			}
			break;
		case 0x46524341: // ACRF
			{
				auto acrf0001 = layer_node->form("0001");
				load_affector<affector_color_ramp_fractal>(acrf0001, acrf0001->form("DATA")->record("PARM"), parent);
			}
			break;
		case 0x48524341: // ACRH
			{
				auto acrh0000 = layer_node->form("0000");
				load_affector<affector_color_ramp_height>(acrh0000, acrh0000->record("DATA"), parent);
			}
			break;
		case 0x564e4541: // AENV
			{
				auto aenv0000 = layer_node->form("0000");
				load_affector<affector_environment>(aenv0000, aenv0000->record("DATA"), parent);
			}
			break;
		case 0x43584541: // AEXC
			{
				auto aexc0000 = layer_node->form("0000");
				load_affector<affector_exclude>(aexc0000, aexc0000->record("DATA"), parent);
			}
			break;
		case 0x4e444641: // AFDN
			{
				auto afdn0002 = layer_node->form("0002");
				load_affector<affector_radial_near_constant>(afdn0002, afdn0002->record("DATA"), parent);
			}
			break;
		case 0x46444641: // AFDF
			{
				auto afdf0002 = layer_node->form("0002");
				load_affector<affector_radial_far_constant>(afdf0002, afdf0002->record("DATA"), parent);
			}
			break;
		case 0x43534641: // AFSC
			{
				auto afsc0004 = layer_node->form("0004");
				load_affector<affector_flora_collidable_constant>(afsc0004, afsc0004->record("DATA"), parent);
			}
			break;
		case 0x4e534641: // AFSN
			{
				auto afsn0004 = layer_node->form("0004");
				load_affector<affector_flora_non_collidable_constant>(afsn0004, afsn0004->record("DATA"), parent);
			}
			break;
		case 0x4e434841: // AHCN
			{
				auto ahcn0000 = layer_node->form("0000");
				load_affector<affector_height_constant>(ahcn0000, ahcn0000->record("DATA"), parent);
			}
			break;
		case 0x52464841: // AHFR
			{
				auto ahfr0003 = layer_node->form("0003");
				load_affector<affector_height_fractal>(ahfr0003, ahfr0003->form("DATA")->record("PARM"), parent);
			}
			break;
		case 0x52544841: // AHTR
			{
				auto ahtr0004 = layer_node->form("0004");
				load_affector<affector_height_terrace>(ahtr0004, ahtr0004->record("DATA"), parent);
			}
			break;
		case 0x56495241: // ARIV
			{
				auto ariv0005 = layer_node->form("0005");
				auto affector = load_affector<affector_river>(ariv0005, ariv0005->form("DATA")->record("DATA"), parent);
				load_height_data<affector_river>(affector, ariv0005->form("DATA")->form("HDTA")->form("0001"));
			}
			break;
		case 0x414f5241: // AROA
			{
				auto aroa0005 = layer_node->form("0005");
				auto affector = load_affector<affector_road>(aroa0005, aroa0005->form("DATA")->record("DATA"), parent);
				load_height_data<affector_road>(affector, aroa0005->form("DATA")->form("HDTA")->form("0001"));
			}
			break;
		case 0x4e435341: // ASCN
			{
				auto ascn0001 = layer_node->form("0001");
				load_affector<affector_shader_constant>(ascn0001, ascn0001->record("DATA"), parent);
			}
			break;
		case 0x50525341: // ASRP
			{
				auto asrp0001 = layer_node->form("0001");
				load_affector<affector_shader_replace>(asrp0001, asrp0001->record("DATA"), parent);
			}
			break;
		case 0x52494342: // BCIR
			{
				auto bcir0002 = layer_node->form("0002");
				load_boundary<boundary_circle>(bcir0002, bcir0002->record("DATA"), parent);
			}
			break;
		case 0x4e4c5042: // BPLN
			{
				auto bpln0001 = layer_node->form("0001");
				load_boundary<boundary_polyline>(bpln0001, bpln0001->record("DATA"), parent);
			}
			break;
		case 0x4c4f5042: // BPOL
			{
				auto bpol0005 = layer_node->form("0005");
				load_boundary<boundary_polygon>(bpol0005, bpol0005->record("DATA"), parent);
			}
			break;
		case 0x43455242: // BREC
			{
				auto brec0003 = layer_node->form("0003");
				load_boundary<boundary_rectangle>(brec0003, brec0003->record("DATA"), parent);
			}
			break;
		case 0x52494446: // FDIR
			{
				auto fdir0000 = layer_node->form("0000");
				load_filter<filter_direction>(fdir0000, fdir0000->record("DATA"), parent);
			}
			break;
		case 0x41524646: // FFRA
			{
				auto ffra0005 = layer_node->form("0005");
				load_filter<filter_fractal>(ffra0005, ffra0005->form("DATA")->record("PARM"), parent);
			}
			break;
		case 0x54474846: // FHGT
			{
				auto fhgt0002 = layer_node->form("0002");
				load_filter<filter_height>(fhgt0002, fhgt0002->record("DATA"), parent);
			}
			break;
		case 0x44485346: // FSHD
			{
				auto fshd0000 = layer_node->form("0000");
				load_filter<filter_shader>(fshd0000, fshd0000->record("DATA"), parent);
			}
			break;
		case 0x504c5346: // FSLP
			{
				auto fslp0002 = layer_node->form("0002");
				load_filter<filter_slope>(fslp0002, fslp0002->record("DATA"), parent);
			}
			break;
		default:
			{
				throw std::runtime_error("Unknown layer type: " + layer_node->str_form_type());
			}
		}
	}

	static std::unique_ptr<construction_layer> load_construction_layer(iff_node* layer_node, construction_layer* parent = nullptr)
	{
		auto layr0003 = layer_node->form("0003");

		auto layer = load_layer<construction_layer>(layr0003, layr0003->record("ADTA"));

		for (uint32_t i = 2; i < layr0003->children.size(); ++i)
		{
			load_layer(layr0003->children[i].get(), layer.get());
		}

		if (parent)
		{
			parent->containers.push_back(std::move(layer));
			layer = nullptr;
		}

		return layer;
	}

	template<typename T>
	static std::unique_ptr<T> load_layer(iff_node* layer_node, iff_node* data_node)
	{
		auto layer = std::make_unique<T>();

		static_cast<base_terrain_layer*>(layer.get())->deserialize(layer_node->form("IHDR")->form("0001")->record("DATA")->data);
		layer->deserialize(data_node->data);

		return layer;
	}

	template<typename T>
	static T* load_affector(iff_node* layer_node, iff_node* data_node, construction_layer* parent)
	{
		auto layer = load_layer<T>(layer_node, data_node);

		T* result = layer.get();

		parent->affectors.push_back(std::move(layer));

		return result;
	}

	template<typename T>
	static T* load_boundary(iff_node* layer_node, iff_node* data_node, construction_layer* parent)
	{
		auto layer = load_layer<T>(layer_node, data_node);

		T* result = layer.get();

		parent->boundaries.push_back(std::move(layer));

		return result;
	}

	template<typename T>
	static T* load_filter(iff_node* layer_node, iff_node* data_node, construction_layer* parent)
	{
		auto layer = load_layer<T>(layer_node, data_node);

		T* result = layer.get();

		parent->filters.push_back(std::move(layer));

		return result;
	}

	template<typename T>
	static void load_height_data(T* layer, iff_node* height_data_node)
	{
		for (const auto& child : height_data_node->children)
		{
			auto segment = std::make_unique<T::segment>();
			segment->deserialize(child->data);

			layer->segments.push_back(std::move(segment));
		}
	}
};

struct terrain_iff_writer
{
	static void store_header(procedural_terrain& terrain, iff_node* parent)
	{
		auto data_record = swganh::tre::make_record("DATA", parent);

		terrain.header.serialize(data_record->data);

		parent->children.push_back(std::move(data_record));
	}

	static void store_footer(procedural_terrain& terrain, iff_node* parent)
	{
		auto form0001 = swganh::tre::make_form("0001", parent);

		auto data_record = swganh::tre::make_record("DATA", form0001.get());
		auto wmap_record = swganh::tre::make_record("WMAP", form0001.get());
		auto smap_record = swganh::tre::make_record("SMAP", form0001.get());

		terrain.footer.serialize(data_record->data);
		terrain.water_map.serialize(wmap_record->data);
		terrain.slope_map.serialize(smap_record->data);

		form0001->children.push_back(std::move(data_record));
		form0001->children.push_back(std::move(wmap_record));
		form0001->children.push_back(std::move(smap_record));

		parent->children.push_back(std::move(form0001));
	}
};

std::unique_ptr<procedural_terrain> swganh::tre::read_procedural_terrain(ByteBuffer& buffer)
{
	auto iff_doc = parse_iff(buffer);

	auto pt = std::make_unique<procedural_terrain>();

	auto form0014 = iff_doc->form("0014");

	if (form0014)
	{
		terrain_iff_reader::load_header(*pt, form0014->record("DATA"));
		terrain_iff_reader::load_terrain_data(*pt, form0014->form("TGEN"));
		terrain_iff_reader::load_footer(*pt, form0014->form("0001"));
	}

	return pt;
}

ByteBuffer swganh::tre::write_procedural_terrain(procedural_terrain& terrain)
{
	auto iff_doc = swganh::tre::make_version_form("PTAT", "0014");
	auto form0014 = iff_doc->form("0014");

	terrain_iff_writer::store_header(terrain, form0014);
	terrain_iff_writer::store_footer(terrain, form0014);

	swganh::ByteBuffer out_buffer;

	write_iff(out_buffer, iff_doc.get());

	return out_buffer;
}
