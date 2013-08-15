
#pragma once

#include <memory>
#include <vector>
#include <QAbstractItemModel>

namespace swganh {
namespace tre {
    struct procedural_terrain;

namespace detail_terrain
{
    struct base_terrain_layer;
}}}


namespace swganh {

class LayerModel : public QAbstractItemModel
{
public:
    LayerModel(swganh::tre::procedural_terrain& terrain, QObject* parent = nullptr);
    ~LayerModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    swganh::tre::detail_terrain::base_terrain_layer* layerFromIndex(const QModelIndex& index) const;

    swganh::tre::procedural_terrain& terrain_;
};

}
