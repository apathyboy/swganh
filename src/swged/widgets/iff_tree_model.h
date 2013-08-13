
#pragma once

#include <QAbstractItemModel>

#include "swganh/byte_buffer.h"

Q_DECLARE_METATYPE(swganh::ByteBuffer*);

namespace swganh {
namespace tre {
    struct iff_node;
}}

class IffTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit IffTreeModel(QObject* parent = nullptr);

    void setHeadNode(swganh::tre::iff_node* head);

    // READ ONLY

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    swganh::tre::iff_node* nodeFromIndex(const QModelIndex& index) const;

    swganh::tre::iff_node* head_;
};
