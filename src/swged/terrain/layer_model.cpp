
#include "layer_model.h"

#include <QPixmap>

#include "swganh/terrain/terrain_types.h"

using swganh::LayerModelIconType;
using swganh::LayerModel;
using swganh::terrain::base_affector_layer;
using swganh::terrain::base_boundary_layer;
using swganh::terrain::base_filter_layer;
using swganh::terrain::base_terrain_layer;
using swganh::terrain::boundary_rectangle;
using swganh::terrain::boundary_polygon;
using swganh::terrain::construction_layer;
using swganh::terrain::e_layer_type;

LayerModel::LayerModel(std::vector<std::unique_ptr<construction_layer>>& layers, QObject* parent)
    : QAbstractItemModel(parent)
    , layers_(layers)
    , layer_icons_(":/icons/layer_icons.png")
{
    layers_.size();
}

LayerModel::~LayerModel()
{}

QModelIndex LayerModel::index(int row, int column, const QModelIndex& parent) const
{
    auto parent_layer = layerFromIndex(parent);

	if (parent_layer)
	{
		return createIndex(row, column, parent_layer->children[row].get());
	}
	else
	{
		return createIndex(row, column, layers_[row].get());
	}
}

QModelIndex LayerModel::parent(const QModelIndex& index) const
{
    auto layer = layerFromIndex(index);

    if (!layer)
    {
        return QModelIndex();
    }

    auto parent_layer = layer->parent;

    if (parent_layer)
    {
        int row = 0;

        if (auto grandparent_layer = parent_layer->parent)
        {
            auto find_iter = std::find_if(grandparent_layer->children.begin(),
                grandparent_layer->children.end(),
                [parent_layer] (const std::unique_ptr<base_terrain_layer>& child)
            {
                return child.get() == parent_layer;
            });

            row = find_iter - grandparent_layer->children.begin();
        }
        else
        {
            auto find_iter = std::find_if(layers_.begin(),
                layers_.end(),
                [parent_layer] (const std::unique_ptr<construction_layer>& child)
            {
                return child.get() == parent_layer;
            });

            row = find_iter - layers_.begin();
        }

        return createIndex(row, index.column(), parent_layer);
    }

    return QModelIndex();
}

int LayerModel::rowCount(const QModelIndex& parent) const
{
    if (auto parent_layer = layerFromIndex(parent))
    {
        return parent_layer->children.size();
    }
	else
	{
		return layers_.size();
	}

    return 0;
}

int LayerModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

QVariant LayerModel::data(const QModelIndex& index, int role) const
{
    auto layer = layerFromIndex(index);

    if (!layer)
    {
        return QVariant();
    }

    if (role == Qt::CheckStateRole && index.column() == 0)
    {
        return static_cast<int>(layer->enabled ? Qt::Checked : Qt::Unchecked);
    }

    if (role == Qt::DecorationRole && index.column() == 0)
    {
        return getLayerIconPixmap(layer);
    }

    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    return QString::fromStdString(layer->name);
}

Qt::ItemFlags LayerModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flags { Qt::ItemIsEnabled | Qt::ItemIsSelectable };

    if (index.column() == 0)
    {
        flags |= Qt::ItemIsUserCheckable;
    }

    return flags;
}

base_terrain_layer* LayerModel::layerFromIndex(const QModelIndex& index) const
{
    if (index.isValid())
    {
        return static_cast<base_terrain_layer*>(index.internalPointer());
    }

    return nullptr;
}

QPixmap LayerModel::getLayerIconPixmap(swganh::terrain::base_terrain_layer* layer) const
{
    LayerModelIconType offset = LayerModelIconType::unknown;
    uint32_t width = 16;
    uint32_t height = 16;

    switch (layer->get_layer_type())
    {
    case swganh::terrain::e_layer_type::affector:
        { offset = getAffectorOffset(layer); }
        break;
    case swganh::terrain::e_layer_type::boundary:
        { offset = getBoundaryOffset(layer); }
        break;
    case swganh::terrain::e_layer_type::construction:
        { offset = getFolderOffset(layer); }
        break;
    case swganh::terrain::e_layer_type::filter:
        { offset = getFilterOffset(layer); }
        break;
    default:
        {}
    }

    return QPixmap::fromImage(layer_icons_.copy(width * static_cast<uint32_t>(offset), 0, width, height));
}

LayerModelIconType LayerModel::getAffectorOffset(swganh::terrain::base_terrain_layer* layer) const
{
    auto affector = static_cast<base_affector_layer*>(layer);
    LayerModelIconType offset = LayerModelIconType::unknown;

    switch (affector->get_type())
    {
    case swganh::terrain::e_affector_type::height_constant:
    case swganh::terrain::e_affector_type::height_fractal:
    case swganh::terrain::e_affector_type::height_terrace:
        { offset = LayerModelIconType::affector_height; }
        break;
    case swganh::terrain::e_affector_type::color_constant:
    case swganh::terrain::e_affector_type::color_ramp_fractal:
    case swganh::terrain::e_affector_type::color_ramp_height:
        { offset = LayerModelIconType::affector_color; }
        break;
    case swganh::terrain::e_affector_type::shader_constant:
    case swganh::terrain::e_affector_type::shader_replace:
        { offset = LayerModelIconType::affector_shader;}
        break;
    case swganh::terrain::e_affector_type::flora_static_collidable_constant:
    case swganh::terrain::e_affector_type::flora_static_non_collidable_constant:
        { offset = LayerModelIconType::affector_flora_static;}
        break;
    case swganh::terrain::e_affector_type::flora_dynamic_near_constant:
    case swganh::terrain::e_affector_type::flora_dynamic_far_constant:
        { offset = LayerModelIconType::affector_flora_dynamic; }
        break;
    case swganh::terrain::e_affector_type::exclude:
        { offset = LayerModelIconType::affector_exclude; }
        break;
    case swganh::terrain::e_affector_type::road:
        { offset = LayerModelIconType::affector_road; }
        break;
    case swganh::terrain::e_affector_type::river:
        { offset = LayerModelIconType::affector_river; }
        break;
    case swganh::terrain::e_affector_type::environment:
        { offset = LayerModelIconType::affector_environment; }
        break;
    default:
        {}
    }

    return offset;
}

LayerModelIconType LayerModel::getFolderOffset(swganh::terrain::base_terrain_layer* layer) const
{
    auto folder = static_cast<construction_layer*>(layer);
    LayerModelIconType offset = LayerModelIconType::unknown;

    if (folder)
    {
        if (folder->invert_boundaries && folder->invert_filters)
        {
            offset = LayerModelIconType::layer_invert_boundaries_filters;
        }
        else if (folder->invert_boundaries)
        {
            offset = LayerModelIconType::layer_invert_boundaries;
        }
        else if (folder->invert_filters)
        {
            offset = LayerModelIconType::layer_invert_filters;
        }
        else
        {
            offset = LayerModelIconType::layer;
        }
    }

    return offset;
}

LayerModelIconType LayerModel::getBoundaryOffset(swganh::terrain::base_terrain_layer* layer) const
{
    auto boundary = static_cast<base_boundary_layer*>(layer);
    LayerModelIconType offset = LayerModelIconType::unknown;

    switch (boundary->get_type())
    {
    case swganh::terrain::e_boundary_type::rectangle:
        {
            auto rectangle = static_cast<boundary_rectangle*>(layer);
            if (rectangle && rectangle->is_local_water_table)
            {
                offset = LayerModelIconType::boundary_rectangle_water_table;
            }
            else
            {
                offset = LayerModelIconType::boundary_rectangle;
            }
        }
        break;
    case swganh::terrain::e_boundary_type::circle:
        { offset = LayerModelIconType::boundary_circle; }
        break;
    case swganh::terrain::e_boundary_type::polygon:
        {
            auto polygon = static_cast<boundary_polygon*>(layer);
            if (polygon && polygon->is_local_water_table)
            {
                offset = LayerModelIconType::boundary_polygon_water_table;
            }
            else
            {
                offset = LayerModelIconType::boundary_polygon;
            }
        }
        break;
    case swganh::terrain::e_boundary_type::polyline:
        { offset = LayerModelIconType::boundary_polyline; }
        break;
    default:
        {}
    }

    return offset;
}

LayerModelIconType LayerModel::getFilterOffset(swganh::terrain::base_terrain_layer* layer) const
{
    auto filter = static_cast<base_filter_layer*>(layer);
    LayerModelIconType offset = LayerModelIconType::unknown;

    switch (filter->get_type())
    {
    case swganh::terrain::e_filter_type::slope:
        { offset = LayerModelIconType::filter_slope; }
        break;
    case swganh::terrain::e_filter_type::direction:
        { offset = LayerModelIconType::filter_direction; }
        break;
    case swganh::terrain::e_filter_type::height:
        { offset = LayerModelIconType::filter_height; }
        break;
    case swganh::terrain::e_filter_type::fractal:
        { offset = LayerModelIconType::filter_fractal; }
        break;
    case swganh::terrain::e_filter_type::shader:
        { offset = LayerModelIconType::filter_shader; }
        break;
    default:
        {}
    }

    return offset;
}
