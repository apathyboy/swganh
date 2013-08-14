
#include "archive_model.h"

using swganh::ArchiveModel;

ArchiveModel::ArchiveModel(std::vector<std::string> filenames, QObject* parent)
    : QAbstractItemModel(parent)
    , filenames_(std::move(filenames))
{}

ArchiveModel::~ArchiveModel()
{}

QModelIndex ArchiveModel::index(int row, int column, const QModelIndex& parent) const
{
    return QModelIndex();
}

QModelIndex ArchiveModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}

int ArchiveModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

int ArchiveModel::columnCount(const QModelIndex& parent) const
{
    return 2;
}

QVariant ArchiveModel::data(const QModelIndex& index, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }

    auto filename = filenames_[index.internalId()];

    if (index.column() == 0)
    {
        return QString::fromStdString(filename);
    }
    {
        return QString::fromStdString(filename.substr(filename.find_last_of(".")));
    }

    return QVariant();
}
