
#include "layer_model.h"

#include "swganh/terrain/procedural_terrain.h"
#include "swganh/terrain/terrain_types.h"

using swganh::LayerModel;
using swganh::terrain::procedural_terrain;
using swganh::terrain::base_terrain_layer;


LayerModel::LayerModel(procedural_terrain& terrain, QObject* parent)
    : QAbstractItemModel(parent)
    , terrain_(terrain)
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
    auto i = terrain_.header.chunk_width;
    return nullptr;
}
