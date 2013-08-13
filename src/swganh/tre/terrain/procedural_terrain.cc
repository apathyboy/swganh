
#include "procedural_terrain.h"

#include <vector>

#include "swganh/tre/iff/iff.h"

using namespace swganh::tre::detail_terrain;
using swganh::ByteBuffer;
using swganh::tre::iff_node;
using swganh::tre::procedural_terrain;

#ifdef WIN32
using std::make_unique;
#else
using swganh::make_unique;
#endif

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
			auto family = make_unique<T>();
			family->deserialize(child->data);

			group.add_family(std::move(family));
		}
	}

	static void load_group(terrain_group<environment_family>& group, iff_node* group_node)
	{
		for (const auto& child : group_node->children)
		{
			auto family = make_unique<environment_family>();
			family->deserialize(child->record("DATA")->data);

			group.add_family(std::move(family));
		}
	}

	static void load_group(terrain_group<fractal_family>& group, iff_node* group_node)
	{
		for (const auto& child : group_node->children)
		{
			auto data = child->record("DATA");
			auto fractal = child->form("MFRC")->form("0001")->record("DATA");

			auto fractal_fam = make_unique<fractal_family>();
			fractal_fam->deserialize(data->data);
			fractal_fam->fractal_data = make_unique<fractal_family::fractal>();
			fractal_fam->fractal_data->deserialize(fractal->data);

			group.add_family(std::move(fractal_fam));
		}
	}

	static void load_layers(std::vector<std::unique_ptr<construction_layer>> &layers, iff_node* lyrs)
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
			parent->children.push_back(std::make_pair(layer.get(), swganh::tre::detail_terrain::e_layer_type::construction));
			parent->containers.push_back(std::move(layer));
			layer = nullptr;
		}

		return layer;
	}

	template<typename T>
	static std::unique_ptr<T> load_layer(iff_node* layer_node, iff_node* data_node)
	{
		auto layer = make_unique<T>();

		static_cast<base_terrain_layer*>(layer.get())->deserialize(layer_node->form("IHDR")->form("0001")->record("DATA")->data);
		layer->deserialize(data_node->data);

		return layer;
	}

	template<typename T>
	static T* load_affector(iff_node* layer_node, iff_node* data_node, construction_layer* parent)
	{
		auto layer = load_layer<T>(layer_node, data_node);

		T* result = layer.get();

		parent->children.push_back(std::make_pair(layer.get(), swganh::tre::detail_terrain::e_layer_type::affector));
		parent->affectors.push_back(std::move(layer));

		return result;
	}

	template<typename T>
	static T* load_boundary(iff_node* layer_node, iff_node* data_node, construction_layer* parent)
	{
		auto layer = load_layer<T>(layer_node, data_node);

		T* result = layer.get();

		parent->children.push_back(std::make_pair(layer.get(), swganh::tre::detail_terrain::e_layer_type::boundary));
		parent->boundaries.push_back(std::move(layer));

		return result;
	}

	template<typename T>
	static T* load_filter(iff_node* layer_node, iff_node* data_node, construction_layer* parent)
	{
		auto layer = load_layer<T>(layer_node, data_node);

		T* result = layer.get();

		parent->children.push_back(std::make_pair(layer.get(), swganh::tre::detail_terrain::e_layer_type::filter));
		parent->filters.push_back(std::move(layer));

		return result;
	}

	template<typename T>
	static void load_height_data(T* layer, iff_node* height_data_node)
	{
		for (const auto& child : height_data_node->children)
		{
			auto segment = make_unique<typename T::segment>();
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

	static void store_terrain_data(procedural_terrain& terrain, iff_node* parent)
	{
		auto tgen = swganh::tre::make_version_form("TGEN", "0000", parent);
		auto tgen0000 = tgen->form("0000");

		store_group(terrain.shader_group, "SGRP", "0006", "SFAM", tgen0000);
		store_group(terrain.flora_group, "FGRP", "0008", "FFAM", tgen0000);
		store_group(terrain.radial_group, "RGRP", "0003", "RFAM", tgen0000);
		store_group(terrain.environment_group, "EGRP", "0002", "EFAM", tgen0000);
		store_group(terrain.fractal_group, "MGRP", "0000", "MFAM", tgen0000);

		store_layers(terrain.layers, tgen0000);

		parent->children.push_back(std::move(tgen));
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

	template<typename T>
	static void store_group(T& group, const char form_type[4], const char form_version[4], const char record_type[4], iff_node* parent)
	{
		auto group_form = swganh::tre::make_version_form(form_type, form_version, parent);
		auto group_version_form = group_form->form(form_version);

		for (const auto& family : group.get_families())
		{
			auto family_form = swganh::tre::make_record(record_type, group_version_form);
			family->serialize(family_form->data);

			group_version_form->children.push_back(std::move(family_form));
		}

		parent->children.push_back(std::move(group_form));
	}

	static void store_group(terrain_group<environment_family>& group, const char form_type[4], const char form_version[4], const char record_type[4], iff_node* parent)
	{
		auto group_form = swganh::tre::make_version_form(form_type, form_version, parent);
		auto group_version_form = group_form->form(form_version);

		for (const auto& family : group.get_families())
		{
			auto family_form = swganh::tre::make_form(record_type, group_version_form);
			auto family_data = swganh::tre::make_record("DATA", family_form.get());

			family->serialize(family_data->data);

			family_form->children.push_back(std::move(family_data));
			group_version_form->children.push_back(std::move(family_form));
		}

		parent->children.push_back(std::move(group_form));
	}

	static void store_group(terrain_group<fractal_family>& group, const char form_type[4], const char form_version[4], const char record_type[4], iff_node* parent)
	{
		auto group_form = swganh::tre::make_version_form(form_type, form_version, parent);
		auto group_version_form = group_form->form(form_version);

		for (const auto& family : group.get_families())
		{
			auto family_form = swganh::tre::make_form(record_type, group_version_form);
			auto family_data = swganh::tre::make_record("DATA", family_form.get());

			auto fractal_form = swganh::tre::make_version_form("MFRC", "0001", family_form.get());
			auto fractal_data = swganh::tre::make_record("DATA", fractal_form->form("0001"));

			family->serialize(family_data->data);
			family->fractal_data->serialize(fractal_data->data);

			fractal_form->form("0001")->children.push_back(std::move(fractal_data));

			family_form->children.push_back(std::move(family_data));
			family_form->children.push_back(std::move(fractal_form));

			group_version_form->children.push_back(std::move(family_form));
		}

		parent->children.push_back(std::move(group_form));
	}

	static void store_layers(std::vector<std::unique_ptr<construction_layer>>& layers, iff_node* parent)
	{
		auto lyrs_form = swganh::tre::make_form("LYRS", parent);

		for (const auto& layer : layers)
		{
			store_construction_layer(layer.get(), lyrs_form.get());
		}

		parent->children.push_back(std::move(lyrs_form));
	}

	static void store_construction_layer(construction_layer* layer, iff_node* parent)
	{
		auto layr = swganh::tre::make_version_form("LAYR", "0003");
		auto layr0003 = layr->form("0003");
		auto data_record = swganh::tre::make_record("ADTA", layr0003);

		store_layer_header(layer, layr0003);
		layer->serialize(data_record->data);

		layr0003->children.push_back(std::move(data_record));

		for (auto child : layer->children)
		{
			switch (child.second)
			{
			case swganh::tre::detail_terrain::e_layer_type::affector:
				{
					store_affector(static_cast<base_affector_layer*>(child.first), layr0003);
				}
				break;
			case swganh::tre::detail_terrain::e_layer_type::boundary:
				{
					store_boundary(static_cast<base_boundary_layer*>(child.first), layr0003);
				}
				break;
			case swganh::tre::detail_terrain::e_layer_type::construction:
				{
					store_construction_layer(static_cast<construction_layer*>(child.first), layr0003);
				}
				break;
			case swganh::tre::detail_terrain::e_layer_type::filter:
				{
					store_filter(static_cast<base_filter_layer*>(child.first), layr0003);
				}
				break;
			default:
				{
					throw std::runtime_error("Invalid layer type detected");
				}
			}
		}

		parent->children.push_back(std::move(layr));
	}

	static void store_affector(base_affector_layer* affector, iff_node* parent)
	{
		switch (affector->get_type())
		{
		case swganh::tre::detail_terrain::e_affector_type::color_constant:
			{
				store_layer(static_cast<affector_color_constant*>(affector), "ACCN", "0000", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::color_ramp_fractal:
			{
				store_parm_layer(static_cast<affector_color_ramp_fractal*>(affector), "ACRF", "0001", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::color_ramp_height:
			{
				store_layer(static_cast<affector_color_ramp_height*>(affector), "ACRH", "0000", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::environment:
			{
				store_layer(static_cast<affector_environment*>(affector), "AENV", "0000", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::exclude:
			{
				store_layer(static_cast<affector_exclude*>(affector), "AEXC", "0000", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::flora_dynamic_near_constant:
			{
				store_layer(static_cast<affector_radial_near_constant*>(affector), "AFDN", "0002", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::flora_dynamic_far_constant:
			{
				store_layer(static_cast<affector_radial_far_constant*>(affector), "AFDF", "0002", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::flora_static_collidable_constant:
			{
				store_layer(static_cast<affector_flora_collidable_constant*>(affector), "AFSC", "0004", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::flora_static_non_collidable_constant:
			{
				store_layer(static_cast<affector_flora_non_collidable_constant*>(affector), "AFSN", "0004", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::height_constant:
			{
				store_layer(static_cast<affector_height_constant*>(affector), "AHCN", "0000", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::height_fractal:
			{
				store_parm_layer(static_cast<affector_height_fractal*>(affector), "AHFR", "0003", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::height_terrace:
			{
				store_layer(static_cast<affector_height_terrace*>(affector), "AHTR", "0004", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::river:
			{
				store_segment_layer(static_cast<affector_river*>(affector), "ARIV", "0005", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::road:
			{
				store_segment_layer(static_cast<affector_road*>(affector), "AROA", "0005", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::shader_constant:
			{
				store_layer(static_cast<affector_shader_constant*>(affector), "ASCN", "0001", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_affector_type::shader_replace:
			{
				store_layer(static_cast<affector_shader_replace*>(affector), "ASRP", "0001", parent);
			}
			break;
		default:
			{
				throw std::runtime_error("Invalid affector type detected");
			}
		}
	}

	static void store_boundary(base_boundary_layer* boundary, iff_node* parent)
	{
		switch (boundary->get_type())
		{
		case swganh::tre::detail_terrain::e_boundary_type::circle:
			{
				store_layer(static_cast<boundary_circle*>(boundary), "BCIR", "0002", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_boundary_type::polyline:
			{
				store_layer(static_cast<boundary_polyline*>(boundary), "BPLN", "0001", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_boundary_type::polygon:
			{
				store_layer(static_cast<boundary_polygon*>(boundary), "BPOL", "0005", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_boundary_type::rectangle:
			{
				store_layer(static_cast<boundary_rectangle*>(boundary), "BREC", "0003", parent);
			}
			break;
		default:
			{
				throw std::runtime_error("Invalid boundary type detected");
			}
		}
	}

	static void store_filter(base_filter_layer* filter, iff_node* parent)
	{
		switch (filter->get_type())
		{
		case swganh::tre::detail_terrain::e_filter_type::direction:
			{
				store_layer(static_cast<filter_direction*>(filter), "FDIR", "0000", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_filter_type::fractal:
			{
				store_parm_layer(static_cast<filter_fractal*>(filter), "FFRA", "0005", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_filter_type::height:
			{
				store_layer(static_cast<filter_height*>(filter), "FHGT", "0002", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_filter_type::shader:
			{
				store_layer(static_cast<filter_shader*>(filter), "FSHD", "0000", parent);
			}
			break;
		case swganh::tre::detail_terrain::e_filter_type::slope:
			{
				store_layer(static_cast<filter_slope*>(filter), "FSLP", "0002", parent);
			}
			break;
		default:
			{
				throw std::runtime_error("Invalid filter type detected");
			}
		}
	}

	template<typename T>
	static void store_segment_layer(T* layer, const char form_type[4], const char form_version[4], iff_node* parent)
	{
		auto layer_form = swganh::tre::make_version_form(form_type, form_version);
		auto version_form = layer_form->form(form_version);
		auto data_form = swganh::tre::make_form("DATA", version_form);
		auto data_record = swganh::tre::make_record("DATA", data_form.get());
		auto height_form = swganh::tre::make_version_form("HDTA", "0001");

		store_layer_header(layer, version_form);
		layer->serialize(data_record->data);

		for (const auto& segment : layer->segments)
		{
			auto segment_record = swganh::tre::make_record("SGMT", height_form->form("0001"));
			segment->serialize(segment_record->data);

			height_form->form("0001")->children.push_back(std::move(segment_record));
		}

		data_form->children.push_back(std::move(height_form));
		data_form->children.push_back(std::move(data_record));

		version_form->children.push_back(std::move(data_form));

		parent->children.push_back(std::move(layer_form));
	}

	template<typename T>
	static void store_layer(T* layer, const char form_type[4], const char form_version[4], iff_node* parent)
	{
		auto layer_form = swganh::tre::make_version_form(form_type, form_version);
		auto version_form = layer_form->form(form_version);
		auto data_record = swganh::tre::make_record("DATA", version_form);

		store_layer_header(layer, version_form);
		layer->serialize(data_record->data);

		version_form->children.push_back(std::move(data_record));

		parent->children.push_back(std::move(layer_form));
	}

	template<typename T>
	static void store_parm_layer(T* layer, const char form_type[4], const char form_version[4], iff_node* parent)
	{
		auto layer_form = swganh::tre::make_version_form(form_type, form_version);
		auto version_form = layer_form->form(form_version);
		auto data_form = swganh::tre::make_form("DATA", version_form);
		auto data_record = swganh::tre::make_record("PARM", data_form.get());

		store_layer_header(layer, version_form);
		layer->serialize(data_record->data);

		data_form->children.push_back(std::move(data_record));
		version_form->children.push_back(std::move(data_form));

		parent->children.push_back(std::move(layer_form));
	}

	static void store_layer_header(base_terrain_layer* layer, iff_node* parent)
	{
		auto ihdr = swganh::tre::make_version_form("IHDR", "0001", parent);
		auto ihdr0001 = ihdr->form("0001");
		auto data = swganh::tre::make_record("DATA", ihdr0001);

		layer->serialize(data->data);

		ihdr0001->children.push_back(std::move(data));

		parent->children.push_back(std::move(ihdr));
	}
};

std::unique_ptr<procedural_terrain> swganh::tre::read_procedural_terrain(ByteBuffer& buffer)
{
	auto iff_doc = parse_iff(buffer);
	auto pt = make_unique<procedural_terrain>();
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
	terrain_iff_writer::store_terrain_data(terrain, form0014);
	terrain_iff_writer::store_footer(terrain, form0014);

	swganh::ByteBuffer out_buffer;

	write_iff(out_buffer, iff_doc.get());

	return out_buffer;
}
