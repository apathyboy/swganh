
#pragma once

#include <memory>
#include <vector>
#include <QAbstractItemModel>

namespace swganh {
namespace terrain {
    struct construction_layer;
    struct base_terrain_layer;
}}


namespace swganh {

class LayerModel : public QAbstractItemModel
{
public:
    LayerModel(std::vector<std::unique_ptr<swganh::terrain::construction_layer>>& layers, QObject* parent = nullptr);
    ~LayerModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    // layer name | layer type | layer sub type
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    swganh::terrain::base_terrain_layer* layerFromIndex(const QModelIndex& index) const;

    std::vector<std::unique_ptr<swganh::terrain::construction_layer>>& layers_;
};

}
