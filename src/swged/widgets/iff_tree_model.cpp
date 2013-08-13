
#include "iff_tree_model.h"

#include "swganh/tre/iff/iff.h"

using swganh::tre::iff_node;

IffTreeModel::IffTreeModel(QObject* parent)
    : QAbstractItemModel(parent)
{}

void IffTreeModel::setHeadNode(iff_node* head)
{
    head_ = head;
}

QModelIndex IffTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!head_)
    {
        return QModelIndex();
    }

    auto parent_node = nodeFromIndex(parent);

    return createIndex(row, column, parent_node->children[row].get());
}

QModelIndex IffTreeModel::parent(const QModelIndex& index) const
{
    auto node = nodeFromIndex(index);

    if (!node)
    {
        return QModelIndex();
    }

    auto parent_node = node->parent;

    if (!parent_node)
    {
        return QModelIndex();
    }

    int row = 0;

    if (auto grandparent_node = parent_node->parent)
    {
        auto find_iter = std::find_if(grandparent_node->children.begin(),
            grandparent_node->children.end(),
            [parent_node] (const std::unique_ptr<iff_node>& child)
        {
            return child.get() == parent_node;
        });

        row = find_iter - grandparent_node->children.begin();
    }

    return createIndex(row, index.column(), parent_node);
}

int IffTreeModel::rowCount(const QModelIndex& parent) const
{
    auto parent_node = nodeFromIndex(parent);

    if (parent_node)
    {
        return parent_node->children.size();
    }

    return 0;
}

int IffTreeModel::columnCount(const QModelIndex& parent) const
{
    return 2;
}

QVariant IffTreeModel::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    auto node = nodeFromIndex(index);

    if (index.column() == 0)
    {
        return QString::fromStdString(node->str_name());
    }
    else
    {
        if (node->form_type)
        {
            return QString::fromStdString(node->str_form_type());
        }
        else
        {
            return QVariant::fromValue(&node->data);
        }
    }

    return QVariant();
}

iff_node* IffTreeModel::nodeFromIndex(const QModelIndex& index) const
{
    if (index.isValid())
    {
        return static_cast<iff_node*>(index.internalPointer());
    }

    return head_;
}
