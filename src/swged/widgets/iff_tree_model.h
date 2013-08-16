
#pragma once

#include <memory>
#include <QAbstractItemModel>

#include "swganh/byte_buffer.h"

Q_DECLARE_METATYPE(swganh::ByteBuffer*);

namespace swganh {
namespace tre {
    struct iff_node;
}}  // namespace swganh::tre

class IffTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit IffTreeModel(std::unique_ptr<swganh::tre::iff_node> head, QObject* parent = nullptr);
    ~IffTreeModel();

    // READ ONLY

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

private:
    swganh::tre::iff_node* nodeFromIndex(const QModelIndex& index) const;

    std::unique_ptr<swganh::tre::iff_node> head_;
};
