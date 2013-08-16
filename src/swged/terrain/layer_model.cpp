
#include "layer_model.h"

#include "swganh/terrain/terrain_types.h"

using swganh::LayerModel;
using swganh::terrain::base_terrain_layer;
using swganh::terrain::construction_layer;


LayerModel::LayerModel(std::vector<std::unique_ptr<construction_layer>>& layers, QObject* parent)
    : QAbstractItemModel(parent)
    , layers_(layers)
{
    layers_.size();
}

LayerModel::~LayerModel()
{}

QModelIndex LayerModel::index(int row, int column, const QModelIndex& parent) const
{
    auto parent_layer = layerFromIndex(parent);

    return createIndex(row, column, parent_layer->children[row].get());
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

    return 0;
}

int LayerModel::columnCount(const QModelIndex& parent) const
{
    return 3;
}

QVariant LayerModel::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    auto layer = layerFromIndex(index);

    switch (index.column())
    {
    case 0:
        {
            return QString::fromStdString(layer->name);
        }
        break;
    case 1:
        {
            return QString::fromStdString(layer->type_str());
        }
        break;
    case 2:
        {
            return QString::fromStdString(layer->subtype_str());
        }
        break;
    }

    return QVariant();
}

base_terrain_layer* LayerModel::layerFromIndex(const QModelIndex& index) const
{
    if (index.isValid())
    {
        return static_cast<base_terrain_layer*>(index.internalPointer());
    }

    return nullptr;
}
