
#include "layer_model.h"

#include "swganh/terrain/terrain_types.h"

using swganh::LayerModel;
using swganh::terrain::base_terrain_layer;
using swganh::terrain::construction_layer;


LayerModel::LayerModel(std::vector<std::unique_ptr<construction_layer>>& layers, QObject* parent)
    : QAbstractItemModel(parent)
    , layers_(layers)
{}

LayerModel::~LayerModel()
{}

QModelIndex LayerModel::index(int row, int column, const QModelIndex& parent) const
{
    return QModelIndex();
}

QModelIndex LayerModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int LayerModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

int LayerModel::columnCount(const QModelIndex& parent) const
{
    return 0;
}

QVariant LayerModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

base_terrain_layer* LayerModel::layerFromIndex(const QModelIndex& index) const
{
    layers_.size();
    return nullptr;
}
