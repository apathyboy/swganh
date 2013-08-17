
#include "layer_model.h"

#include <QPixmap>

#include "swganh/terrain/terrain_types.h"

using swganh::LayerModel;
using swganh::terrain::base_affector_layer;
using swganh::terrain::base_boundary_layer;
using swganh::terrain::base_filter_layer;
using swganh::terrain::base_terrain_layer;
using swganh::terrain::construction_layer;
using swganh::terrain::e_layer_type;

LayerModel::LayerModel(std::vector<std::unique_ptr<construction_layer>>& layers, QObject* parent)
    : QAbstractItemModel(parent)
    , layers_(layers)
    , layer_icons_(":/icons/layer_icons.bmp")
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
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

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
    uint32_t offset = 0;
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

    return QPixmap::fromImage(layer_icons_.copy(width * offset, 0, width, height));
}

uint32_t LayerModel::getAffectorOffset(swganh::terrain::base_terrain_layer* layer) const
{
    auto affector = static_cast<base_affector_layer*>(layer);
    uint32_t offset = 0;

    switch (affector->get_type())
    {
    case swganh::terrain::e_affector_type::height_constant:
    case swganh::terrain::e_affector_type::height_fractal:
    case swganh::terrain::e_affector_type::height_terrace:
        { offset = 11; }
        break;
    case swganh::terrain::e_affector_type::color_constant:
    case swganh::terrain::e_affector_type::color_ramp_fractal:
    case swganh::terrain::e_affector_type::color_ramp_height:
        { offset = 12; }
        break;
    case swganh::terrain::e_affector_type::shader_constant:
    case swganh::terrain::e_affector_type::shader_replace:
        { offset = 13;}
        break;
    case swganh::terrain::e_affector_type::flora_static_collidable_constant:
    case swganh::terrain::e_affector_type::flora_static_non_collidable_constant:
        { offset = 14;}
        break;
    case swganh::terrain::e_affector_type::flora_dynamic_near_constant:
    case swganh::terrain::e_affector_type::flora_dynamic_far_constant:
        { offset = 15; }
        break;
    case swganh::terrain::e_affector_type::exclude:
        { offset = 16; }
        break;
    case swganh::terrain::e_affector_type::road:
        { offset = 17; }
        break;
    case swganh::terrain::e_affector_type::river:
        { offset = 18; }
        break;
    case swganh::terrain::e_affector_type::environment:
        { offset = 19; }
        break;
    default:
        {}
    }

    return offset;
}

uint32_t LayerModel::getFolderOffset(swganh::terrain::base_terrain_layer* layer) const
{
    return 1;
}

uint32_t LayerModel::getBoundaryOffset(swganh::terrain::base_terrain_layer* layer) const
{
    auto boundary = static_cast<base_boundary_layer*>(layer);
    uint32_t offset = 0;

    switch (boundary->get_type())
    {
    case swganh::terrain::e_boundary_type::rectangle:
        { offset = 2; }
        break;
    case swganh::terrain::e_boundary_type::circle:
        { offset = 3; }
        break;
    case swganh::terrain::e_boundary_type::polygon:
        { offset = 4; }
        break;
    case swganh::terrain::e_boundary_type::polyline:
        { offset = 5; }
        break;
    default:
        {}
    }

    return offset;
}

uint32_t LayerModel::getFilterOffset(swganh::terrain::base_terrain_layer* layer) const
{
    auto filter = static_cast<base_filter_layer*>(layer);
    uint32_t offset = 0;

    switch (filter->get_type())
    {
    case swganh::terrain::e_filter_type::slope:
        { offset = 6; }
        break;
    case swganh::terrain::e_filter_type::direction:
        { offset = 7; }
        break;
    case swganh::terrain::e_filter_type::height:
        { offset = 8; }
        break;
    case swganh::terrain::e_filter_type::fractal:
        { offset = 9; }
        break;
    case swganh::terrain::e_filter_type::shader:
        { offset = 10; }
        break;
    default:
        {}
    }

    return offset;
}
